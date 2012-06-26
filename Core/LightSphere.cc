
#include "Core/LightSphere.h"
#include "Core/BoundingBox.h"
#include "Core/utils.h"
#include "Core/Ray.h"
#include "Core/HitRecord.h"
#include "Core/RenderContext.h"
#include "Geometry/Sphere.h"

LightSphere::LightSphere(const Point &center, float radius, const Color& color,
			 unsigned int samples)
  : _center(center), _radius(radius), _color(color), _samples(samples)
{
}

LightSphere::~LightSphere()
{
}

void LightSphere::preprocess()
{
}

double LightSphere::getLight(Color& light_color, Vector& light_direction,
			     const RenderContext& context, const Point& hitpos) const
{
  light_color = _color;

  // Get coords
  double x = drand48();
  double y = drand48();
  double z = drand48();
#if 0
  Vector v(drand48(), drand48(), drand48() );
  Ray ray( hitpos, v);
  HitRecord h(_radius * 2.0);
  Sphere s( NULL, _center, _radius );
  s.intersect( h, RenderContext(NULL), ray );
  light_direction = -v;
  light_direction.normalize();
  
  return Vector(h.hitpos - hitpos).normalize();
#else
  Vector v(x*2 - 1.0,
	   y*2 - 1.0,
	   z*2 - 1.0);
  double len = v.normalize();
  light_direction = v;
  return len;
#endif
}

void
LightSphere::getRays( std::vector<Ray> &rays, int numRays, const BoundingBox &bounds ) const {
  rays.reserve(numRays);
  
  for ( int i = 0; i < numRays; ++i ) {
    Vector dir( Vector::Rand() );
    dir.normalize();
    Point p = _center + dir * _radius;
    rays[i] = Ray( p,  dir );
  }
}

void
LightSphere::getPhoton(Color &color, Ray &photon) const
{
  color = _color;

  Vector dir;
  do {
    dir = -Vector::Rand();
  } while (dir.length2() <= 1.0 );

  // Yes, minus - this flips the direction of the ray from in to out.
  photon.set( _center - _radius*dir, dir );
}
