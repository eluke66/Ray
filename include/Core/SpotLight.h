
#ifndef SpotLight_h
#define SpotLight_h

#include "Core/Light.h"
#include "Core/Vector.h"
#include "Core/Color.h"
#include "Core/Ray.h"
#include "Geometry/Sphere.h"

class SimpleSpotLight : public Light {
 public:
  SimpleSpotLight(const Vector& direction, const Point &center, double radius, const Color& color);
  virtual ~SimpleSpotLight();

  virtual void preprocess();
  virtual double getLight(Color& light_color, Vector& light_direction,
                          const RenderContext& context, const Point& pos) const;

  virtual void getRays( std::vector<Ray> &rays, int numRays, const BoundingBox &bounds ) const {
    rays.reserve(numRays);
    for ( int i = 0; i < numRays; ++i ) {
      rays[i] = Ray( (Point)(_direction * (-1 * 10e6)), _direction );
    }
  }
  virtual void getColor(Color &color) { color = _color; }
  virtual void getPhoton(Color &color, Ray &photon) const;
 protected:
  SimpleSpotLight(const SimpleSpotLight&);
  SimpleSpotLight& operator=(const SimpleSpotLight&);

  Vector _direction;
  Sphere _light;
  Color _color;
};

class SpotLight : public SimpleSpotLight {
public:
  SpotLight( const Vector &direction, const Point &center, double falloff, const Color &color, double radius );
  virtual ~SpotLight();

  virtual double getLight(Color& light_color, Vector& light_direction,
                          const RenderContext& context, const Point& pos) const;
  virtual void getPhoton(Color &color, Ray &photon) const;
protected:
  double _falloff;
};
#endif

