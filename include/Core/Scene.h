
#ifndef Scene_h
#define Scene_h

#include "Core/Color.h"
#include <string>
#include <vector>
#include <map>

#include "Core/PointLight.h"

class Background;
class Camera;
class Color;
class Filter2D;
class Image;
class Light;
class Object;
class RenderContext;
class Ray;
class UI;
class Scene;
class PhotonMap;
class FrameFunc;

typedef Scene *(*SceneMaker)();
typedef std::map< std::string, SceneMaker > SceneMakerMap;

class Scene {
 public:
  Scene();
  virtual ~Scene();

  inline Object* getObject() const {
    return object;
  }
  void setObject(Object* obj) {
    object = obj;
  }

  inline Background* getBackground() const {
    return background;
  }
  void setBackground(Background* bg) {
    background = bg;
  }

  inline Camera* getCamera() const {
    return camera;
  }
  void setCamera(Camera* cam) {
    camera = cam;
  }

  void setImage(Image* im) {
    image = im;
  }

  void addLight(Light* light) {
    lights.push_back(light);
  }
  const std::vector<Light*>& getLights() const {
    return lights;
  }

  Color getAmbient() const {
    return ambient;
  }
  void setAmbient(const Color& amb) {
    ambient = amb;
  }

  int getMaxRayDepth() const {
    return maxRayDepth;
  }
  void setMaxRayDepth(int rd) {
    maxRayDepth = rd;
  }
  double getMinAttenuation() const {
    return minAttenuation;
  }
  void setMinAttenuation(double atten) {
    minAttenuation = atten;
  }

  enum SampleType {
    Uniform,
    Jittered
  };
  int getNumSamples() const {
    return numSamples;
  }
  void setNumSamples(int samples) {
    numSamples = samples;
  }
  SampleType getSampleType() const {
    return sampleType;
  }
  void setSampleType(SampleType newSampleType) {
    sampleType = newSampleType;
  }
  void setSampling(SampleType newSampleType, int samples) {
    sampleType = newSampleType;
    numSamples = samples;
  }
  static void PrintSamplerTypes();
  static SampleType LookupSampler( std::string samplerName );
  
  Filter2D* getFilter() const;
  void setFilter(Filter2D* filter);

  void addFrameManipulator( FrameFunc *func ) { _frameFuncs.push_back(func); }
  const std::vector<FrameFunc*>& getFrameManipulators() const
  { return _frameFuncs; }
  void preprocess();
  void render();
  void render( int xstart, int ystart, int xend, int yend, UI *ui );
  double traceRay(Color& result, RenderContext& context, const Ray& ray, const Color& attenuation, int depth) const;
  double traceRay(Color& result, RenderContext& context, const Object* obj, const Ray& ray, const Color& attenuation, int depth) const;

  PhotonMap *getPhotonMap( bool global=true ) const { return _photonMap; }
  PhotonMap *getCausticPhotonMap( bool global=true ) const { return _causticPhotonMap; }
  
 private:
  Scene(const Scene&);
  Scene& operator=(const Scene&);

  Background* background;
  Camera* camera;
  Color ambient;
  Image* image;
  Object* object;
  Filter2D* filter;
  std::vector<Light*> lights;
  int maxRayDepth;
  double minAttenuation;
  std::vector<FrameFunc*> _frameFuncs;
  int numSamples;
  SampleType sampleType;
  PhotonMap *_photonMap;
  PhotonMap *_causticPhotonMap;

  void _globalIllumination();
  void _emitPhoton( PhotonMap * map, bool storeFirstPhoton, Light *light,
		    unsigned numPhotons );
  void _tracePhoton( PhotonMap *map, const Color &color, const Ray &photon,
		     bool storeThisPhoton, int depth );
  
  typedef std::map<std::string, SampleType> SampleNameMap;
  static SampleNameMap _SamplerTypes;
  static bool _LoadedSamplers;
  static void LoadSamplers();
};

#endif
