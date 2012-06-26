
#ifndef DirectionalLight_h
#define DirectionalLight_h

#include "Core/Light.h"
#include "Core/Vector.h"
#include "Core/Color.h"
#include "Core/Ray.h"

class DirectionalLight : public Light {
 public:
  DirectionalLight(const Vector& direction, const Color& color);
  virtual ~DirectionalLight();

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
 private:
  DirectionalLight(const DirectionalLight&);
  DirectionalLight& operator=(const DirectionalLight&);

  Vector _direction;
  Color _color;
};

#endif

