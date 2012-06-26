
#ifndef VolumeMaterial_h
#define VolumeMaterial_h

#include "Core/Array3.h"
#include "Material/Material.h"
#include "Core/Color.h"
#include "Core/Colormap.h"
#include "Core/Point.h"
#include "Core/Vector.h"
#include "IO/VolumeReader.h"
#include "Core/utils.h"
#include <float.h>
#include "Material/Utilities.h"
#include "Core/Scene.h"
#include "Core/RenderContext.h"
#include "Core/Ray.h"

template <typename LightShader>
class VolumeMaterial {
 public:
  VolumeMaterial(const std::string& headername,
		 const std::string& cmapname,
		 const Point& lower, const Point& upper,
		 double grid_stepsize, float maxopacity, float Ka,
		 LightShader shader ) :
    _cmap(cmapname), _lower(lower), _maxopacity(maxopacity), _Ka(Ka), _shader(shader)
  {
    _diag = upper-lower;
    VolumeReader::ReadVolume( headername, _diag,
			      grid_stepsize, _world_stepsize, _cellsize, _data );
    _cmap.rescale(_world_stepsize);
  }
  
  virtual ~VolumeMaterial() {}

  void operator()(Color& result, RenderContext& context, const Ray& ray,
		  const HitRecord& hit, const Color& atten, int depth) const {
    const Scene* scene = context.getScene();
    const Object* world = scene->getObject();
    const std::vector<Light*>& lights = scene->getLights();
    Light*const* end = &lights[lights.size()];
    Light*const* begin = &lights[0];

    LightShader shader(_shader);

    
    Point hitpos = ray.origin()+ray.direction()*hit.minT();
    
    Ray exitray(hitpos, ray.direction());
    HitRecord exithit(DBL_MAX);
    hit.getPrimitive()->intersect(exithit, context, exitray);
    if(!exithit.getPrimitive()){
      // Cannot find exit point, just return...
      result = Color(0,0,0);
      return;
    }
    double texit = hit.minT() + exithit.minT();

    Color amblight = scene->getAmbient()*_Ka;
    int it = Ceil(hit.minT()/_world_stepsize);
    double t = it*_world_stepsize;
    double accum_opacity = 0;
    Color accum_color(0, 0, 0);
    while(t < texit && accum_opacity < _maxopacity){
      INC_STAT(RaysTransmitted);
      Point p(ray.origin()+t*ray.direction());
      Vector tp((p-_lower)/_cellsize);
      int ix = (int)tp.x();
      int iy = (int)tp.y();
      int iz = (int)tp.z();
      float fx = tp.x()-ix;
      float fy = tp.y()-iy;
      float fz = tp.z()-iz;
      float value = 
        _data(ix,   iy,   iz  )*(1-fx)*(1-fy)*(1-fz)
	+ _data(ix+1, iy,   iz  )*fx    *(1-fy)*(1-fz)
	+ _data(ix,   iy+1, iz  )*(1-fx)*fy    *(1-fz)
	+ _data(ix+1, iy+1, iz  )*fx    *fy    *(1-fz)
	+ _data(ix,   iy,   iz+1)*(1-fx)*(1-fy)*fz
	+ _data(ix+1, iy,   iz+1)*fx    *(1-fy)*fz
	+ _data(ix,   iy+1, iz+1)*(1-fx)*fy    *fz
	+ _data(ix+1, iy+1, iz+1)*fx    *fy    *fz;
      float opacity;
      Color color;
      _cmap.lookup(value, opacity, color);
      
      if(opacity > 0.0){
	Vector normal(_data(ix+1, iy, iz)-_data(ix-1, iy, iz),
		      _data(ix, iy+1, iz)-_data(ix, iy-1, iz),
		      _data(ix, iy, iz+1)-_data(ix, iy, iz-1));
	normal *= _cellsize;
	if(normal.length2() > 0)
	  normal.normalize();
	else {
	  normal = ray.direction();
	}
	double costheta = -Dot(normal, ray.direction());
	if(costheta < 0){
	  normal = -normal;
	  costheta = -costheta;
	}

	begin = &lights[0];
	shader.color(color);
	shader.normal(normal);
	shader.ray(ray.direction());
	color = amblight * color +
	  MaterialUtilities::GetLightColor( begin, end, context,
					    hitpos, normal, world, shader );
	
	accum_color += color * opacity * (1-accum_opacity);
	accum_opacity += opacity * (1-accum_opacity);
      }
      t += _world_stepsize;
    }

    if(accum_opacity < _maxopacity){
      // Trace a ray at the exit point...
      Point exitpoint(ray.origin()+ray.direction()*texit);
      Ray outsideray(exitpoint, ray.direction());
      Color exit_color;
      double T = 1-accum_opacity;
      scene->traceRay(exit_color, context, outsideray, Color(T, T, T), depth+1);
      INC_STAT(RaysTransmitted);
      accum_color += exit_color * (1-accum_opacity);
    }
    result = accum_color;
  }

 private:
  //  VolumeMaterial(const VolumeMaterial&);
  //VolumeMaterial& operator=(const VolumeMaterial&);

  Colormap _cmap;
  Point _lower;
  Vector _diag;
  Vector _cellsize;
  double _world_stepsize;
  float _maxopacity;
  Array3<short> _data;
  float _Ka;
  LightShader _shader;
};

#endif
