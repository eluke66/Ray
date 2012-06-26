
#ifndef PointLight_h
#define PointLight_h

#include "Core/Light.h"
#include "Core/Point.h"
#include "Core/Color.h"

class PointLight;

struct LightPower {
  bool operator()( const PointLight &l1, const PointLight &l2 );
};

class PointLight : public Light {
  friend class LightPower;
 public:
  PointLight(const Point& position, const Color& color);
  PointLight(const PointLight&);
  PointLight& operator=(const PointLight&p) {
    position = p.position;
    color    = p.color;
    return *this;
  }
  virtual ~PointLight();

  virtual void preprocess();
  virtual double getLight(Color& light_color, Vector& light_direction,
                          const RenderContext& context, const Point& pos) const;
  virtual void getRays( std::vector<Ray> &rays, int numRays, const BoundingBox &bounds ) const;

  const Point &getPosition() const { return position; }
  const Color &getColor() const { return color; }
  virtual void getColor(Color &color) { color = this->color; }
  virtual void getPhoton(Color &color, Ray &photon) const;
 private:

  Point position;
  Color color;
};

#endif

