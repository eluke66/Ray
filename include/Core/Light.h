
#ifndef Light_h
#define Light_h

class Color;
class Point;
class Ray;
class RenderContext;
class Vector;
class BoundingBox;

#include <vector>

class Light {
 public:
  Light();
  virtual ~Light();

  virtual void preprocess();
  virtual double getLight(Color& light_color, Vector& light_direction,
                          const RenderContext& context, const Point& pos) const = 0;
  virtual unsigned int samples() { return 1; }
  virtual void getRays( std::vector<Ray> &rays, int numRays, const BoundingBox &bounds  ) const = 0;

  virtual void getColor(Color &color) = 0;
  virtual void getPhoton(Color &color, Ray &photon) const = 0;
 private:
  Light(const Light&);
  Light& operator=(const Light&);
};

#endif

