
#include "Core/AreaLight.h"
#include "Core/BoundingBox.h"
#include "Core/utils.h"
#include "Core/Ray.h"
#include "Core/Matrix.h"

AreaLight::AreaLight(const Primitive &geometry, const Color& color, unsigned int samples)
  : _geometry(geometry), _color(color), _samples(samples)
{
}

AreaLight::~AreaLight()
{
}

void AreaLight::preprocess()
{
}

double AreaLight::getLight(Color& light_color, Vector& light_direction,
                            const RenderContext& context, const Point& hitpos) const
{
  light_color = _color;

  BoundingBox b;
  _geometry.getBounds(b);
  Vector boundSize = b.getMax() - b.getMin();
  // Get coords
  double x = drand48();
  double y = drand48();
  double z = drand48();
  
  Point location = b.getMin() + Vector(boundSize.x() * x,
				       boundSize.y() * y,
				       boundSize.z() * z);
  Vector dir = location-hitpos;
  double len = dir.normalize();
  light_direction = dir;
  return len;
}

void AreaLight::getRays( std::vector<Ray> &rays, int numRays, const BoundingBox &bounds ) const {
  
  
}

void AreaLight::getPhoton(Color &color, Ray &photon) const
{
  assert(0);
}

PlaneLight::PlaneLight(const Plane& plane,
		       const Color& color,
		       unsigned int samples) :
  AreaLight(plane,color,samples), _plane(plane) {}

double
PlaneLight::getLight(Color& light_color, Vector& light_direction,
		     const RenderContext& context, const Point& pos) const
{
  light_color = _color;

  // Pick a point on the plane randomly
  Point onPlane;
  _plane.randomPoint(onPlane);

  // Find the vector between this point and pos.
  light_direction = onPlane-pos;
  double len = light_direction.normalize();
  
  // Scale the power of the light by the cosine of the vector and the
  // plane normal (the dot product of these two vectors).
  double dot = -Dot(_plane.normal(), light_direction);
  light_color = _color * dot;

  // Return the distance to the light
  return len;

}

void PlaneLight::getPhoton(Color &color, Ray &photon) const
{
  color = _color;

  // Pick a point on the plane randomly
  Point onPlane;
  _plane.randomPoint(onPlane);

  // Find a cosine weighted vector, and set up the photon.
  photon.set( onPlane,
	      Vector::CosineWeightedVector( _plane.normal() ) );
}



SphereLight::SphereLight(const Sphere& sphere,
			 const Color& color,
			 unsigned int samples) :
  AreaLight(sphere,color,samples), _sphere(sphere) {}

double
SphereLight::getLight(Color& light_color, Vector& light_direction,
		     const RenderContext& context, const Point& pos) const
{
  light_color = _color;

  // Pick a point on the sphere randomly
  Point onSphere;
  _sphere.randomPoint(onSphere);

  // Find the vector between this point and pos.
  light_direction = onSphere-pos;
  double len = light_direction.normalize();
  
  // Scale the power of the light by the cosine of the vector and the
  // sphere normal (the dot product of these two vectors).
  double dot = -Dot(_sphere.normal(onSphere), light_direction);
  light_color = _color * dot;

  // Return the distance to the light
  return len;

}

void SphereLight::getPhoton(Color &color, Ray &photon) const
{
  color = _color;

  // Pick a point on the sphere randomly
  Point onSphere;
  _sphere.randomPoint(onSphere);

  // Find a cosine weighted vector, and set up the photon.
  photon.set( onSphere,
	      Vector::CosineWeightedVector( _sphere.normal(onSphere) ) );
}
