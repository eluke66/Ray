
#ifndef AreaLight_h
#define AreaLight_h

#include "Core/Light.h"
#include "Geometry/Primitive.h"
#include "Geometry/Plane.h"
#include "Geometry/Sphere.h"
#include "Core/Color.h"

class AreaLight : public Light {
 public:
  AreaLight(const Primitive& geometry, const Color& color, unsigned int samples);
  virtual ~AreaLight();

  virtual void preprocess();
  virtual double getLight(Color& light_color, Vector& light_direction,
                          const RenderContext& context, const Point& pos) const;
  virtual unsigned int samples() { return _samples; }
  virtual void getRays( std::vector<Ray> &rays, int numRays, const BoundingBox &bounds ) const;
  
  virtual void getColor(Color &color) { color = _color; }
  virtual void getPhoton(Color &color, Ray &photon) const;
 protected:
  AreaLight(const AreaLight&);
  AreaLight& operator=(const AreaLight&);

  const Primitive &_geometry;
  Color _color;
  unsigned int _samples;
};

class PlaneLight : public AreaLight {
public:
  PlaneLight(const Plane& plane, const Color& color, unsigned int samples);

  virtual double getLight(Color& light_color, Vector& light_direction,
                          const RenderContext& context, const Point& pos) const;
  
  virtual void getPhoton(Color &color, Ray &photon) const;
 private:
  PlaneLight(const PlaneLight&);
  PlaneLight& operator=(const PlaneLight&);

  const Plane& _plane;
};

class SphereLight : public AreaLight {
public:
  SphereLight(const Sphere& sphere, const Color& color, unsigned int samples);

  virtual double getLight(Color& light_color, Vector& light_direction,
                          const RenderContext& context, const Point& pos) const;
  
  virtual void getPhoton(Color &color, Ray &photon) const;
 private:
  SphereLight(const SphereLight&);
  SphereLight& operator=(const SphereLight&);

  const Sphere& _sphere;
};
#endif

