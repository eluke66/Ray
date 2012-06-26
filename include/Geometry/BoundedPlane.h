
#ifndef BoundedPlane_h
#define BoundedPlane_h

#include "Geometry/Plane.h"

class BoundedPlane : public Plane {
 public:
  BoundedPlane(Material* material, const Vector& n, const Point& point,
	       const Point &pmin, const Point& pmax);
  virtual ~BoundedPlane();

  virtual void transform(const Matrix4 &matrix);
  virtual void getBounds(BoundingBox& bbox) const;
  virtual void intersect(HitRecord& hit, const RenderContext& context, const Ray& ray) const;
  virtual void computeUVW(Vector& uvw, const RenderContext& context,
                          const Ray& ray, const HitRecord& hit) const;

  virtual void randomPoint(Point &pt) const;
 private:
  BoundedPlane(const BoundedPlane&);
  BoundedPlane& operator=(const BoundedPlane&);

  Point _min, _max;
};

#endif

