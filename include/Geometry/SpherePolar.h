
#ifndef SpherePolar_h
#define SpherePolar_h

#include "Geometry/Sphere.h"

class SpherePolar : public Sphere {
 public:
  SpherePolar(Material* material, const Point& center, double radius,
              const Vector& poleAxis, const Vector& primeMeridian);

  virtual void computeUVW(Vector& uvw, const RenderContext& context,
                          const Ray& ray, const HitRecord& hit) const;

 private:
  Vector X, Y, Z;
};

#endif
