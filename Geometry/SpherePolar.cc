
#include "Geometry/SpherePolar.h"
#include "Core/HitRecord.h"
#include "Core/Ray.h"

SpherePolar::SpherePolar(Material* material, const Point& center, double radius,
                         const Vector& poleAxis, const Vector& primeMeridian)
  : Sphere(material, center, radius), X(primeMeridian),
    Y(Cross(poleAxis, primeMeridian)), Z(poleAxis)
{
  ALLOC;
  DALLOC(Sphere);
  X.normalize();
  Y.normalize();
  Z.normalize();
}

void SpherePolar::computeUVW(Vector& uvw, const RenderContext& context,
                             const Ray& ray, const HitRecord& hit) const
{
  Point hitpos = ray.origin() + ray.direction()*hit.minT();
  Vector p = hitpos-center;
  double z = Dot(p, Z);
  double y = Dot(p, Y);
  double x = Dot(p, X);
  double R = sqrt(x*x+y*y+z*z);
  double theta = atan2(y, x);
  theta *= 1./(2*M_PI);
  double u = theta < 0? theta + 1 : theta;
  double phi = acos(z/R);
  double v = 1-phi/M_PI;
  uvw = Vector(u, v, R);
}
