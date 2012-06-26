
#ifndef LightSphere_h
#define LightSphere_h

#include "Core/Light.h"
#include "Core/Color.h"
#include "Core/Point.h"

class LightSphere : public Light {
 public:
  LightSphere(const Point &center, float radius, const Color& color, unsigned int samples);
  virtual ~LightSphere();
  
  virtual void preprocess();
  virtual double getLight(Color& light_color, Vector& light_direction,
                          const RenderContext& context, const Point& pos) const;
  virtual unsigned int samples() { return _samples; }
  virtual void getRays( std::vector<Ray> &rays, int numRays, const BoundingBox &bounds ) const;
  virtual void getColor(Color &color) { color = _color; }
  virtual void getPhoton(Color &color, Ray &photon) const;
  
 private:
  LightSphere(const LightSphere&);
  LightSphere& operator=(const LightSphere&);

  Point _center;
  float _radius;
  Color _color;
  unsigned int _samples;
};

#endif

