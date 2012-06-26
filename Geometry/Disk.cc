
#include "Geometry/Disk.h"
#include "Core/BoundingBox.h"
#include "Core/HitRecord.h"
#include "Core/Math.h"
#include "Core/Ray.h"
#include "Core/Matrix.h"

Disk::Disk(Material* material, const Point& center, const Vector& normal, double radius)
  : Primitive(material), center(center), n(normal), radius2(radius*radius)
{
  ALLOC;
  n.normalize();
  d = Dot(n, center);
}

Disk::~Disk()
{
}


void Disk::transform(const Matrix4 &matrix) {
  center = matrix * center;
  n = matrix * n;
  n.normalize();
  d = Dot(n,center);
}

void Disk::getBounds(BoundingBox& bbox) const
{
  double radius = sqrt(radius2);
  Vector diag(radius*sqrt(n.y()*n.y()+n.z()*n.z()),
              radius*sqrt(n.x()*n.x()+n.z()*n.z()),
              radius*sqrt(n.x()*n.x()+n.y()*n.y()));
  bbox.extend(center+diag);
  bbox.extend(center-diag);
}

void Disk::intersect(HitRecord& hit, const RenderContext& context, const Ray& ray) const
{
  double denom = Dot(n, ray.direction());
  if(Abs(denom) > 1.e-6){
    double t = (d-Dot(n, ray.origin()))/denom;
    Point hitpos = ray.origin()+ray.direction()*t;
    Vector r = hitpos-center;
    if(r.length2() < radius2)
      hit.hit(t, this, matl);
  }
}

void Disk::getNormal(Vector& normal, RenderContext&,
                   const Point&, const Ray&, const HitRecord&) const
{
  normal = n;
}
