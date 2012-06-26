
#include "Geometry/BoundedPlane.h"
#include "Core/HitRecord.h"
#include "Core/Math.h"
#include "Core/Ray.h"
#include "Core/Statistics.h"
#include "Core/Matrix.h"
#include "Core/BoundingBox.h"
#include "Core/utils.h"

BoundedPlane::BoundedPlane(Material* material, const Vector& normal, const Point& point,
			   const Point &pmin, const Point& pmax)
  : Plane(material, normal, point), _min(pmin), _max(pmax)
{
  ALLOC;
  DALLOC(Plane);
}

BoundedPlane::~BoundedPlane()
{
}

void BoundedPlane::transform(const Matrix4 &matrix) {
  Plane::transform(matrix);
  _min = matrix * _min;
  _max = matrix * _max;
}

void BoundedPlane::intersect(HitRecord& hit, const RenderContext& context, const Ray& ray) const
{
  INC_STAT(PlaneIntersectionTests);
  double denom = Dot(n, ray.direction());
  if(Abs(denom) > 1.e-6){
    double t = (d-Dot(n, ray.origin()))/denom;
    Point hitPos = ray.origin() + t * ray.direction();
    if ( hitPos.x() >= _min.x() && hitPos.x() <= _max.x() &&
	 hitPos.y() >= _min.y() && hitPos.y() <= _max.y() &&
	 hitPos.z() >= _min.z() && hitPos.z() <= _max.z() ) {
      hit.hit(t, this, matl);
      INC_STAT(PlaneSuccessfulIntersections);
    }
  }
}

void BoundedPlane::getBounds(BoundingBox& bbox) const
{
  bbox.extend(_min);
  bbox.extend(_max);
}

void BoundedPlane::computeUVW(Vector& uvw, const RenderContext& context,
		       const Ray& ray, const HitRecord& hit) const
{
#ifndef NEW_WAY
  // Might want to fix this...
  Primitive::computeUVW(uvw, context, ray, hit);
  uvw += 1e3;
  uvw = Vector( fabs(uvw.x() - trunc(uvw.x())),
		fabs(uvw.y() - trunc(uvw.y())),
		uvw.z() );
#else

  // Find the hitpoint.
  Primitive::computeUVW(uvw, context, ray, hit);
  uvw *= 7.3;
  
  // Now project u,v,w to by interpolating through the bounds
  uvw =  (uvw - (Vector)_min) / (_max - _min);

   uvw = Vector( fabs(uvw.x() - trunc(uvw.x())),
		 fabs(uvw.y() - trunc(uvw.y())),
		 uvw.z() );
  //uvw.normalize();
#endif
}

void
BoundedPlane::randomPoint(Point &pt) const {
  Vector change = _max-_min;

  if ( p.z() == 0 && p.x() == 0 ) {
    pt = Point( _min.x() + change.x() * drand48(),
		-d-0.001,
		_min.z() + change.z() * drand48() );
  }
  else if ( p.z() == 0 && p.y() == 0 ) {
    pt = Point( -d-0.001,
		_min.y() + change.y() * drand48(),
	        _min.z() + change.z() * drand48() );
  }
  else if ( p.x() == 0 && p.y() == 0 ) {
    pt = Point( _min.x() + change.x() * drand48(),
		_min.y() + change.y() * drand48(),
	        -d-0.001 );
  }
  else {
    std::cerr << "P is " << p << std::endl;
    assert(!"THIS DOESNT WORK YET!");
    PRINT_TIMES(_max << " " << _min, 10);
    // Satisfies px + py + pz = d
    if ( p.z() != 0.0 ) {
      double x = _min.x() + change.x() * drand48();
      double y = _min.y() + change.y() * drand48();
      PRINT_TIMES("\tY1 is " << y, 10);
      double z = (d - p.x()*x+ p.y()*y) / p.z();
      pt = Point( x, y, z);
    }
    else if ( p.x() != 0 ) {
      double z = _min.z() + change.z() * drand48();
      double y = _min.y() + change.y() * drand48();
      double x = (d - p.z()*z+ p.y()*y) / p.x();
      PRINT_TIMES("\tY2 is " << y, 10);
      pt = Point( x, y, z);
    }
    else {
      double x = _min.x() + change.x() * drand48();
      double z = _min.z() + change.z() * drand48();
      // py = d - px - pz
      // y = Z / p
      double y = (d - p.x()*x+ p.z()*z) / p.y();
      PRINT_TIMES("\tY3 is " << y, 10);
      PRINT_TIMES("\tD IS " << d , 10 );
      pt = Point( x, y, z);
    }
  }

  //double dot = Dot(Vector(this->p), Vector(pt));
  //assert( ABSCLOSE(dot, -d, 1e-6));
}
