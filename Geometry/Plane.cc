
#include "Geometry/Plane.h"
#include "Core/HitRecord.h"
#include "Core/Math.h"
#include "Core/Ray.h"
#include "Core/Statistics.h"
#include "Core/Matrix.h"

Plane::Plane(Material* material, const Vector& normal, const Point& point)
  : Primitive(material), n(normal), p(point)
{
  ALLOC;
  n.normalize();
  d = Dot(n, point);
}

Plane::~Plane()
{
}

void Plane::transform(const Matrix4 &matrix) {
  n = matrix * n;
  n.normalize();
  p = matrix * p;
  d = Dot(n,p);
}

void Plane::intersect(HitRecord& hit, const RenderContext& context, const Ray& ray) const
{
  INC_STAT(PlaneIntersectionTests);
  double denom = Dot(n, ray.direction());
  if(Abs(denom) > 1.e-6){
    double t = (d-Dot(n, ray.origin()))/denom;
    hit.hit(t, this, matl);
    INC_STAT(PlaneSuccessfulIntersections);
  }
}

void Plane::getNormal(Vector& normal, RenderContext&,
                   const Point&, const Ray&, const HitRecord&) const
{
  normal = n;
}

void Plane::getBounds(BoundingBox& bbox) const
{
}

void Plane::computeUVW(Vector& uvw, const RenderContext& context,
		       const Ray& ray, const HitRecord& hit) const
{
  Primitive::computeUVW(uvw, context, ray, hit);
  uvw += 1e3;
  uvw = Vector( fabs(uvw.x() - trunc(uvw.x())),
		fabs(uvw.y() - trunc(uvw.y())),
		uvw.z() );
}

void
Plane::randomPoint(Point &pt) const {
  // Satisfies px + py + pz = d
  if ( p.z() != 0.0 ) {
    double x = 1e4 * drand48();
    double y = 1e4 * drand48();
    double z = -(p.x()*x+ p.y()*y - d) / p.z();
    pt = Point( x, y, z);
  }
  else if ( p.x() != 0 ) {
    double z = 1e4 * drand48();
    double y = 1e4 * drand48();
    double x = -(p.z()*z+ p.y()*y - d) / p.x();
    pt = Point( x, y, z);
  }
  else {
    double x = 1e4 * drand48();
    double z = 1e4 * drand48();
    double y = -(p.x()*x+ p.z()*z - d) / p.y();
    pt = Point( x, y, z);
  }
}
