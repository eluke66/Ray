#ifndef InstantRadiosity_h
#define InstantRadiosity_h

#include "Core/RenderContext.h"
#include "Core/Ray.h"
#include "Core/HitRecord.h"
#include "Core/BoundingBox.h"
#include "Material/Material.h"
#include "Geometry/Object.h"
#include <float.h>

const unsigned MAX_BOUNCES = 3;
const unsigned NUM_RADIOSITY_LIGHTS = 100;
const unsigned NUM_BASE_SAMPLES = 100;
const float MIN_RADIOSITY_FACTOR = ( NUM_BASE_SAMPLES / 1000.0 ) / 255.0;

class InstantRadiosity {
public:
  InstantRadiosity(Scene *scene) : _scene(scene) {}
  typedef std::vector<PointLight> LightVector;
  void addRadiosityLights();
  void computeRadiosity( Light *light, LightVector &candidates,
			 unsigned nBounces );
protected:
  Scene *_scene;
};

void InstantRadiosity::addRadiosityLights() {

  const std::vector<Light *>& lights = _scene->getLights();
  LightVector lightCandidates;
  for ( unsigned i = 0; i < lights.size(); ++i ) {
    computeRadiosity( lights[i], lightCandidates, 0 );
  }
  unsigned numLights = std::min( NUM_RADIOSITY_LIGHTS,
				 (unsigned) lightCandidates.size() );

  std::cerr << "There are " << lightCandidates.size() << " potential lights!" << std::endl;
  //std::sort( lightCandidates.begin(), it, LightPower() );
  std::cerr << "picking lights of " << numLights << "...." << std::flush;
  // Add up to this number of lights
  Vector v;
  Color lc;
  RenderContext rc(_scene);
  
  for ( unsigned i = 0; i < numLights; ++i ) {
    _scene->addLight( new PointLight(lightCandidates[i]) );
    lightCandidates[i].getLight( lc, v, rc, Point(0,0,0) );
  }

}
void InstantRadiosity::computeRadiosity( Light * light,
					 LightVector &candidates,
					 unsigned nBounces ) {
  
  if ( nBounces > MAX_BOUNCES )
    return;
  nBounces++;
  RenderContext context(_scene);
  unsigned nSamples = NUM_BASE_SAMPLES / (nBounces * nBounces * nBounces);
  
  // Get light ray samples
  std::vector<Ray> rays;
  BoundingBox bounds;
  _scene->getObject()->getBounds( bounds );
  Point bc = bounds.getCenter();
  //  double bs = bounds.getSize() / 2.0;
  //std::cerr << "Scene bounds: " << bounds << " : getting " << nSamples << " samples " << std::endl;
  light->getRays( rays, nSamples, bounds );
  //std::cerr << " got rays... " << std::endl;
  for ( unsigned i = 0; i < rays.size(); ++i ) {
    HitRecord hit(DBL_MAX);

    _scene->getObject()->intersect(hit, context, rays[i]);
    //std::cerr << "MINT : "<< hit.minT() << std::endl;
    //assert( hit.getPrimitive() );
    if ( hit.getPrimitive() ) {
      //std::cerr << "hit" << std::endl;
      Point hitpoint = rays[i].origin() + rays[i].direction() * hit.minT();
      Color colorAt;
      hit.getMaterial()->shade(colorAt, context, rays[i], hit, Color(1,1,1), 0);
      colorAt /= (double)(nSamples / 1.0f);
      if ( colorAt.power() > MIN_RADIOSITY_FACTOR ) {
	PointLight p( hitpoint, colorAt );
	candidates.push_back( p );
	computeRadiosity( &p, candidates, nBounces + 1 );
      }
    }
  }
}

#endif
