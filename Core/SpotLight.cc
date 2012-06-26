
#include "Core/SpotLight.h"
#include "Core/utils.h"
#include "Core/HitRecord.h"
#include "Geometry/Plane.h"

SimpleSpotLight::SimpleSpotLight(const Vector& direction, const Point &center,
				 double radius,
				 const Color& color)
  : _direction(direction), _light(NULL,center,radius), _color(color)
{
  _direction.normalize();
}

SimpleSpotLight::~SimpleSpotLight()
{
}

void SimpleSpotLight::preprocess()
{
  
}

double SimpleSpotLight::getLight(Color& light_color, Vector& light_direction,
                            const RenderContext& context, const Point& hitpos) const
{
  // Intersect our light sphere with a ray from the hit position. If
  // it hits, color is the light color. Else, it's black (no
  // contribution).
  light_direction = -_direction;
  Ray incoming( hitpos, -_direction );
  HitRecord hit;
  _light.intersect( hit, context, incoming );
  if ( hit.getPrimitive() ) {
    light_color = _color;
    return (hitpos - _light.getCenter()).length();
  }
  else {
    light_color = Color(0.0);
    return FLT_MAX;
  }
}

void
SimpleSpotLight::getPhoton(Color &color, Ray &photon) const
{
  color = _color;

  // This should be on the disk, but we'll just get some random point.
  Point p;
  _light.randomPoint(p);
  photon.set( p, _direction);

}

SpotLight::SpotLight( const Vector &direction, const Point &center,
		      double falloff, const Color &color, double radius ) :
  SimpleSpotLight(direction,center,radius,color), _falloff(-falloff)
{

}

SpotLight::~SpotLight()
{

}

double 
SpotLight::getLight(Color& light_color, Vector& light_direction,
		    const RenderContext& context, const Point& pos) const
{
  // First, see if the point falls in the "full power" section.
  Ray incoming( pos, -_direction );
  HitRecord hit;
  _light.intersect( hit, context, incoming );
  if ( hit.getPrimitive() ) {
    light_color = _color;
    return (pos - _light.getCenter()).length();
  }
  
  // Create a ray from the point to the nearest spot on the sphere's
  // disk:
  // * Find plane orthogonal to light direction (PRECOMPUTE)
  Plane plane(NULL, _direction, _light.getCenter());
  
  // * Intersect ray from point with light direction to plane.
  plane.intersect( hit, context, incoming );
  if ( !hit.getPrimitive() ) {
    // Behind us - no light!
    light_color = Color(0.0);
    return FLT_MAX;
  }
  
  // * Find the vector from this point to the center.
  Point hitOnPlane = incoming.origin() + hit.minT() * incoming.direction();
  
  // * Subtract the center from the hitpos, giving us the 3D distance
  // from center to hit position.
  Vector centerToHit = _light.getCenter() - hitOnPlane;
  
  // * Scale this distance by the radius - this gives us the point on
  // the circle.
  centerToHit *= _light.getRadius() / centerToHit.length();
  
  // * Create a vector from the point to the circle point.
  Vector posToLight = (_light.getCenter() + centerToHit) - pos;
  double distance = posToLight.normalize();

  // Ensure this ray can hit the light - sanity check!
  hit = HitRecord();
  _light.intersect( hit, context, Ray(pos, posToLight) );
  if ( !hit.getPrimitive() ) {
    double dot = Dot( posToLight, _direction );
    if ( dot > 0.0 ) {
      std::cerr << "BAD!" << std::endl;
      std::cerr << Ray(pos, posToLight) << ", Light = " << _light.getCenter() << std::endl;
      assert ( hit.getPrimitive() );
    }
  }

  
  // Find the dot product of this direction and the spotlight's
  // direction.
  double dot = Dot( posToLight, -_direction );
  if ( dot < 0.0 ) {
    std::cerr << "Dot: " << dot << std::endl;
    std::cerr << "\tLight Ray: " << posToLight << "\n\tLight Dir: " << _direction << std::endl;
    assert(0);
  }
  
  // Scale the power of the light by an exponential dropoff
  double power = exp( _falloff * (1.0 - dot) );
  assert( power <= 1.0 );
  
  // And fill in the return values
  light_direction = posToLight;
  light_color = _color * power;

  return distance;
}

void 
SpotLight::getPhoton(Color &color, Ray &photon) const
{
  NOT_DONE("SpotLight::getPhoton");
}
