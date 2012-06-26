
#include "Core/Scene.h"
#include "Background/Background.h"
#include "Core/Camera.h"
#include "Core/Options.h"
#include "Core/HitRecord.h"
#include "Filter/Filters.h"
#include "Core/Image.h"
#include "Core/Light.h"
#include "Material/Material.h"
#include "Geometry/Object.h"
#include "Core/Ray.h"
#include "Core/RenderContext.h"
#include "Filter/Sampler.h"
#include "UI/UserInterface.h"
#include "UI/NilUI.h"
#include "Core/Statistics.h"
#include "Core/BoundingBox.h"
#include "Geometry/Primitive.h"
#include "GlobalIllum/PhotonMap.h"
#include "Geometry/GriddedGroup.h"
#include "Geometry/Sphere.h"
#include "Material/BasicShade.h"
#include "GlobalIllum/Photon.h"
#include "Core/Time.h"
#include "Core/utils.h"
#include "Core/FrameFunc.h"
#include <typeinfo>
#include <float.h>
#include <iostream>
#include <stdlib.h>
using namespace std;

Scene::SampleNameMap
Scene::_SamplerTypes;
bool
Scene::_LoadedSamplers = false;

const int PHOTON_DEPTH = 10; // Max # photon bounces.

// Proportion of scene to check for estimate 
const double BOUND_SIZE_FRACTION = 1.0 / 50.0;

void
Scene::LoadSamplers() {
  if ( !_LoadedSamplers ) {
    _SamplerTypes["uniform"] = Uniform;
    _SamplerTypes["jittered"] = Jittered;
    _LoadedSamplers = true;
  }
}

Scene::Scene()
{
  object = 0;
  background = 0;
  camera = 0;
  ambient = Color(0, 0, 0);
  image = 0;
  minAttenuation = 0;
  maxRayDepth = 2;
  numSamples = 1;
  sampleType = Uniform;
  filter = new Filter2DWrapper<SeparableKernel2D<BoxKernel1D> >;
  _photonMap = NULL;
  _causticPhotonMap = NULL;
  
  LoadSamplers();
  
}

Scene::~Scene()
{
  delete object;
  delete background;
  delete camera;
  delete image;
  for(int i=0;i<static_cast<int>(lights.size());i++){
    Light* light = lights[i];
    delete light;
  }
  delete filter;
}

void Scene::setFilter(Filter2D* new_filter)
{
  if(filter)
    delete filter;
  filter = new_filter;
}

Filter2D* Scene::getFilter() const
{
  return filter;
}

void Scene::preprocess()
{
  assert(background);
  assert(image);
  assert(camera);
  assert(object);
  
  background->preprocess();
  
  for(int i=0;i<static_cast<int>(lights.size());i++){
    Light* light = lights[i];
    light->preprocess();
  }
  double aspect_ratio = image->aspect_ratio();
  camera->preprocess(aspect_ratio);
  object->preprocess();

#define GLOBAL_ILLUM
#ifdef GLOBAL_ILLUM
  if ( Options::Instance()->numPhotons() > 0 ||
       Options::Instance()->numCausticPhotons() > 0 )
    _globalIllumination();
#endif

  for ( std::vector<FrameFunc*>::iterator it = _frameFuncs.begin();
	it != _frameFuncs.end();
	++it ) {
    (*it)->doRegister(this);
  }
}

void
Scene::_globalIllumination() {

  Timer shootTimer;
  unsigned numPhotons = Options::Instance()->numPhotons();
  unsigned numCausticPhotons = Options::Instance()->numCausticPhotons();

  BoundingBox bounds;
  object->getBounds(bounds);
  double boundSize = bounds.getSize();
  
  // If global illumination is specified, do that.
  if ( numPhotons > 0 ) {
    _photonMap = new PhotonMap( numPhotons );
    // If no size estimate has been set up, do that now based on the
    // size of the scene.
    if ( Options::Instance()->estimateRadius() == 0.0 )
      Options::Instance()->estimateRadius( boundSize * BOUND_SIZE_FRACTION );
    std::cerr << "Estimate radius size is " << Options::Instance()->estimateRadius() << std::endl;
  }
  if ( numCausticPhotons > 0 ) {
    _causticPhotonMap = new PhotonMap( numCausticPhotons, true );
    // If no size estimate has been set up, do that now based on the
    // size of the scene.
    if ( Options::Instance()->causticEstimateRadius() == 0.0 )
      Options::Instance()->causticEstimateRadius( boundSize * BOUND_SIZE_FRACTION );
    std::cerr << "Caustic estimate radius size is " << Options::Instance()->causticEstimateRadius() << std::endl;
  }

  // Find the total power of all lights
  Color totalColor(0.0);
  std::vector<Color> lightColors;
  lightColors.resize( lights.size() );
  
  for(int i=0;i<static_cast<int>(lights.size());i++){
    lights[i]->getColor(lightColors[i]);
    totalColor += lightColors[i];
  }

  // Now, re-iterate through the lights emitting photons. Each light
  // gets a different number of photons.
  std::cerr << "Shooting photons..." << std::flush;
  for(int i=0;i<static_cast<int>(lights.size());i++){
    float factor = lightColors[i].power() / totalColor.power();

    if ( numPhotons > 0 ) {
      std::cerr << "G" << std::flush;
      _emitPhoton( _photonMap, false, lights[i], static_cast<int>(numPhotons * factor) );
      std::cerr << "!" << std::flush;
    }

    if ( numCausticPhotons > 0 ) {
      std::cerr << "C" << std::flush;
      _emitPhoton( _causticPhotonMap, false, lights[i], static_cast<int>(numCausticPhotons * factor) );
      std::cerr << "!" << std::flush;
    }
  }

  std::cerr << "Photon shoot took " << shootTimer.time() << " seconds" << std::endl;
  
  ////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////
  // Test for now
  if ( 0 ) {
    Timer viewTimer;
    int nPhotons;
    const Photon * photons = _photonMap->getPhotons(nPhotons);
    GriddedGroup * g = new GriddedGroup(10);
    for ( int i = 0; i < nPhotons; ++i )
      g->addObject( new Sphere(new BasicShade(photons[i].color() * 1e5),
			       photons[i].pos(), 1 ) );
    
    g->preprocess();
    //setObject( g );
    //((Group *)object)->addObject(g);
    std::cerr << "Test preprocessing took " << viewTimer.time() << " seconds." << std::endl;
  }
  // End of test for now
  ////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////

  Timer preTimer;
  if ( numPhotons > 0 )
    _photonMap->preprocess();
  if ( numCausticPhotons > 0 )
    _causticPhotonMap->preprocess();
  
  std::cerr << "Photon map preprocessing took " << preTimer.time()  << " seconds" << std::endl;
}

void
Scene::_emitPhoton( PhotonMap * map, bool storeFirstPhoton, Light *light, unsigned numPhotons )
{
  // Create a photon coming from the light
  Color color;
  Ray photonFromLight;
  int photonsEmitted;
  int lightPhotons = 0;
  
  do {
    light->getPhoton(color, photonFromLight);
    _tracePhoton(map, color, photonFromLight, storeFirstPhoton, PHOTON_DEPTH );
    map->getPhotons(photonsEmitted);
    lightPhotons++;
  }
  while ((unsigned)photonsEmitted < numPhotons);

  map->scalePower( 1.0 / lightPhotons );
}

void
Scene::_tracePhoton( PhotonMap *map, const Color &color, const Ray &photon, bool storeThisPhoton, int depth )
{

  RenderContext context(this);
  HitRecord hit(DBL_MAX);
  object->intersect( hit, context, photon );
  
  if ( hit.getPrimitive() ) {
    const Material* matl = hit.getMaterial();
    assert( matl );
    
    
    // Get a new reflected/refracted photon. If the function returns
    // false, then there is no new photon.
    Ray newDirection;
    Color newColor;
    bool storePhoton;
    Material::BounceType bounce = matl->bounceRay( newDirection, newColor, storePhoton, hit, photon, context);

    if ( storePhoton && storeThisPhoton ) {
      // Store the photon in the photon map. This is the global map
      // - need other work for a specular map.
      map->store( color,
		  photon.origin() + hit.minT() * photon.direction(),
		  photon.direction() );
    }
    
    // We trace another photon if:
    // The depth is okay, AND
    // If caustic map, and a specular bounce, OR
    // If non-caustic, a diffuse bounce or a specular bounce not on
    // the first bounce.
    if ( depth > 0 &&
	 (  ( map->causticOnly() && bounce == Material::SPECULAR) ||
	    (!map->causticOnly() && bounce == Material::DIFFUSE) || 
	    (!map->causticOnly() && bounce == Material::SPECULAR && storeThisPhoton )
	    ) ) {
      
      // Scale the reflected color by the incoming color - the new
      // color is only the proportion that's reflected!
      newColor *= color;

      // Trace the new photon/ray
      _tracePhoton( map, newColor, newDirection, true, depth - 1 );
    } // Trace another photon
  } // Have a primitive.
}

void
Scene::PrintSamplerTypes() {
  if ( !_LoadedSamplers)
    LoadSamplers();
  for ( SampleNameMap::iterator i = _SamplerTypes.begin();
	i != _SamplerTypes.end();
	++i )
    std::cout << i->first << " " << std::flush;
  std::cout << std::endl;
}

Scene::SampleType
Scene::LookupSampler( std::string samplerName ) {
  if ( !_LoadedSamplers)
    LoadSamplers();
  
  if ( _SamplerTypes.find( samplerName ) != _SamplerTypes.end() )
    return _SamplerTypes[ samplerName ];
  else {
    std::cerr << "Unknown sampler type " << samplerName << "; using uniform" << std::endl;
    return Uniform;
  }
}

struct UISaver {
public:
  UISaver(UI *ui) : _ui(ui) {}
  void operator()(int j, int i, const Color& filteredvalue)
  {
    INC_STAT(PixelsRendered);
    _ui->putPixel(filteredvalue, j, i);
  }
private:
  UI *_ui;
};

#include "Core/utils.h"
struct CastRay {
public:
  CastRay(Camera* camera, Scene* scene, RenderContext& context)
    : _camera(camera), _scene(scene), _context(context), _numSamples(camera->numSamples())
  {
  }
  ~CastRay()
  {
  }

  Color operator()(float x, float y)
  {
    Ray ray;
    Color total;
    for ( int i = 0; i < _numSamples; ++i ) {
      _camera->makeRay(ray, _context, x, y, drand48(), drand48());
      INC_STAT(RaysEye);
      Color result;
      _scene->traceRay(result, _context, ray, Color(1,1,1), 0);
      total += result;
    }
    total /= (float)_numSamples;
    return total;
  }
private:
  Camera* _camera;
  Scene* _scene;
  RenderContext& _context;
  int _numSamples;
};

void Scene::render() {
  assert(image);
  NilUI ui(image->getXresolution(),
	   image->getYresolution());
  
  ui.initialize( 0, NULL );
  render( 0, 0,
	  image->getXresolution(),
	  image->getYresolution(),
	  &ui );
}

void Scene::render(int xstart, int ystart, int xend, int yend, UI *ui)
{
  if(!object || !background || !camera || !image){
    cerr << "Incomplete scene, cannot render!\n";
    exit(1);
  }
   // Add this set of pixels to the global stats.
  ADD_STAT( ScreenPixels, (xend-xstart)*(yend-ystart) );
  
  int xres = image->getXresolution();
  int yres = image->getYresolution();
  RenderContext context(this);
  double dx = 2./xres;
  double xmin = -1. + dx/2.;
  double dy = 2./yres;
  double ymin = -1. + dy/2.;
  Color atten(1,1,1);
  int samples = numSamples;
  int numCameraSamples = camera->numSamples();
  mdebug("hi");
  if(samples == 1 && sampleType == Uniform){
    for(int i=ystart;i<yend;i++){
      //cerr << "y=" << i << '\n';
      double y = ymin + i*dy;
      for(int j=xstart;j<xend;j++){
        double x = xmin + j*dx;
	//	std::cerr << "x=" << j << ", y=" << i << " ";
        Ray ray;
	Color total;
	for ( int s = 0; s < numCameraSamples; ++s ) {
	  camera->makeRay(ray, context, x, y, drand48(), drand48());
	  INC_STAT(RaysEye);
	  HitRecord hit(DBL_MAX);
	  object->intersect(hit, context, ray);
	  Color result;
	  if(hit.getPrimitive()){
	    mdebug("Hit " << typeid(*(hit.getPrimitive())).name() << " at " << (ray.origin()+hit.minT()*ray.direction()));
	    // Ray hit something...
	    const Material* matl = hit.getMaterial();
	    if ( !matl ) {
	      ERROR("No material found for primitive " << typeid(*(hit.getPrimitive())).name());
	    }
	    else
	      matl->shade(result, context, ray, hit, atten, 0);
	  } else {
	    background->getBackgroundColor(result, context, ray);
	  }
	  total += result;
	}
	total /= (float)numCameraSamples;
	mdebug("Color is " << total );
	
	INC_STAT(PixelsRendered);
	ui->putPixel(total, j, i );
      }
    }
  } else if(sampleType == Jittered){
    Sampler<Color> sampler(samples, samples, filter,
                           xres, yres);
    sampler.sampleAndFilterRegion(xstart, xend, ystart, yend, drand48, CastRay(camera, this, context), UISaver(ui));

    
  } else {
    Sampler<Color> sampler(samples, samples, filter,
                           xres, yres);
    sampler.sampleAndFilterRegion(xstart, xend, ystart, yend, uniform, CastRay(camera, this, context), UISaver(ui));
  }
}

double Scene::traceRay(Color& result, RenderContext& context, const Ray& ray, const Color& atten, int depth) const
{
  if(depth >= maxRayDepth || atten.maxComponent() < minAttenuation){
    result = Color(0, 0, 0);
    return 0;
  } else {
    HitRecord hit(DBL_MAX);
    object->intersect(hit, context, ray);
    if(hit.getPrimitive()){mdebug("Hit " << typeid(*(hit.getPrimitive())).name() << " at " << (ray.origin()+hit.minT()*ray.direction()));
      // Ray hit something...
      const Material* matl = hit.getMaterial();
      if ( !matl ) {
	std::cerr << "NO MATL: " << typeid(*(hit.getPrimitive())).name() << std::endl;
      }
      assert(matl);
      matl->shade(result, context, ray, hit, atten, depth);
      return hit.minT();
    } else {
      background->getBackgroundColor(result, context, ray);
      return DBL_MAX;
    }
  }
}

double Scene::traceRay(Color& result, RenderContext& context, const Object* obj, const Ray& ray, const Color& atten, int depth) const
{
  if(depth >= maxRayDepth || atten.maxComponent() < minAttenuation){
    result = Color(0, 0, 0);
    return 0;
  } else {
    HitRecord hit(DBL_MAX);
    obj->intersect(hit, context, ray);
    if(hit.getPrimitive()){mdebug("Hit " << typeid(*(hit.getPrimitive())).name() << " at " << (ray.origin()+hit.minT()*ray.direction()));
      // Ray hit something...
      const Material* matl = hit.getMaterial();
      assert(matl);
      matl->shade(result, context, ray, hit, atten, depth);
      return hit.minT();
    } else {
      background->getBackgroundColor(result, context, ray);
      return DBL_MAX;
    }
  }
}
