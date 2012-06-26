
#ifndef NormalPlane_h
#define NormalPlane_h

#include "Geometry/Plane.h"
#include "Geometry/BoundedPlane.h"
#include "Core/Ray.h"
#include "Core/Vector.h"

class NormalPlane : public Plane {
 public:
  NormalPlane(Material* material, const Vector& n, const Point& point) :
    Plane(material,n,point) {}

  virtual void intersect(HitRecord& hit, const RenderContext& context,
			 const Ray& ray) const {
    // If ray is not pointing opposite the direction of the normal,
    // then don't even try to intersect!
    if ( Dot(n,ray.direction()) > 0 )
      return;

    Plane::intersect(hit,context,ray);
  }
  
 protected:
  NormalPlane(const NormalPlane&);
  NormalPlane& operator=(const NormalPlane&);
};

class NormalBoundedPlane : public BoundedPlane {
 public:
  NormalBoundedPlane(Material* material, const Vector& n, const Point& point,
	       const Point &pmin, const Point& pmax) :
    BoundedPlane(material,n,point,pmin,pmax) {}

  virtual void intersect(HitRecord& hit, const RenderContext& context,
			 const Ray& ray) const {
    // If ray is not pointing opposite the direction of the normal,
    // then don't even try to intersect!
    if ( Dot(n,ray.direction()) > 0 )
      return;

    BoundedPlane::intersect(hit,context,ray);
  }
  
 protected:
  NormalBoundedPlane(const NormalBoundedPlane&);
  NormalBoundedPlane& operator=(const NormalBoundedPlane&);
};

#endif

