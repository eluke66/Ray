
#include "Geometry/Sphere.h"
#include "Core/BoundingBox.h"
#include "Core/HitRecord.h"
#include "Core/Point.h"
#include "Core/Ray.h"
#include "Core/Vector.h"
#include "Core/Statistics.h"
#include "Core/Matrix.h"
#include <math.h>

Sphere::Sphere(Material* material, const Point& center, double radius)
  : Primitive(material), center(center), radius(radius)
{
  ALLOC;
  inv_radius = 1./radius;
}

Sphere::~Sphere()
{
}

void Sphere::getBounds(BoundingBox& bbox) const
{
  Vector diag(radius, radius, radius);
  bbox.extend(center+diag);
  bbox.extend(center-diag);
}

void Sphere::transform(const Matrix4 &matrix) {
  center = matrix * center;
}

void Sphere::intersect(HitRecord& hit, const RenderContext&, const Ray& ray) const
{
  INC_STAT(SphereIntersectionTests);
  Vector O(ray.origin()-center);
  const Vector& V(ray.direction());
  double b = Dot(O, V);
  double c = Dot(O, O)-radius*radius;
  double disc = b*b-c;
  if(disc > 0){
    double sdisc = sqrt(disc);
    double root1 = (-b - sdisc);
    if(!hit.hit(root1, this, matl)){
      double root2 = (-b + sdisc);
      hit.hit(root2, this, matl);
    }
    INC_STAT(SphereSuccessfulIntersections);
  }
}

void Sphere::getNormal(Vector& normal, RenderContext&, const Point& hitpos,
                    const Ray& ray, const HitRecord& hit) const
{
  normal = (hitpos-center)*inv_radius;
}

Vector Sphere::normal(const Point& hitpos) const
{
  return (hitpos-center)*inv_radius;
}

void Sphere::randomPoint( Point &point ) const
{
  Vector v;
  do {
    v = Vector::Rand();
  } while ( v.length2() > 1.0 );

  v.normalize();
  point = center + v * radius;
}
