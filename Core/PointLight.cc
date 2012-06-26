
#include "Core/PointLight.h"
#include "Core/Ray.h"
#include "Core/BoundingBox.h"
#include <iostream>

bool LightPower::operator()( const PointLight &l1, const PointLight &l2 ) {
  return l1.color.power() <= l2.color.power();
}

PointLight::PointLight(const Point& position, const Color& color)
  : position(position), color(color)
{
}

PointLight::PointLight(const PointLight& p) : position(p.position), color(p.color)
{
}

PointLight::~PointLight()
{
}

void PointLight::preprocess()
{
}

double PointLight::getLight(Color& light_color, Vector& light_direction,
                            const RenderContext&, const Point& hitpos) const
{
  light_color = color;
  Vector dir = position-hitpos;
  double len = dir.normalize();
  light_direction = dir;
  return len;
}
#include <iostream>
#include "Core/HitRecord.h"
#include "Core/RenderContext.h"
#include "Geometry/Sphere.h"
#include <float.h>
void
PointLight::getRays( std::vector<Ray> &rays, int numRays, const BoundingBox &bounds ) const {

  Point bc = bounds.getCenter();
  double bs = bounds.getSize() / 2.0;
  Vector boundSize = bounds.getMax() - bounds.getMin();
  
  for ( int i = 0; i < numRays; ++i ) {
    Ray r;
    
    double x = drand48();
    double y = drand48();
    double z = drand48();
    Point location = bounds.getMin() + Vector(boundSize.x() * x,
					      boundSize.y() * y,
					      boundSize.z() * z);
      
    Vector dir = location - position;
    dir.normalize();

    r = Ray( position, dir );
    rays.push_back( r );
  }
}


void
PointLight::getPhoton(Color &color, Ray &photon) const
{
  color = this->color;

  Vector dir;
  do {
    dir = Vector::Rand();
  } while (dir.length2() <= 1.0 );

  photon.set( position, dir );
}
