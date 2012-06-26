
#include "Core/DirectionalLight.h"
#include "Core/utils.h"

DirectionalLight::DirectionalLight(const Vector& direction, const Color& color)
  : _direction(direction), _color(color)
{
  _direction.normalize();
}

DirectionalLight::~DirectionalLight()
{
}

void DirectionalLight::preprocess()
{
  
}

double DirectionalLight::getLight(Color& light_color, Vector& light_direction,
                            const RenderContext& context, const Point& hitpos) const
{
  light_color = _color;
  light_direction = -_direction;
  return FLT_MAX;
}

void
DirectionalLight::getPhoton(Color &color, Ray &photon) const
{
  color = _color;

  //#define TEMP_ONLY_FOR_ROOM
#ifdef TEMP_ONLY_FOR_ROOM
  double span = 7.0 / 3.0;
  
  photon.set( Point( 1e8,
		     drand48() * span - span/2.0,
		     drand48() * span - span/2.0),
	      _direction );
#else
  photon.set( Point(-1e8*_direction), _direction);
#endif
}
