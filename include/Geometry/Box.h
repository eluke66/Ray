
#ifndef Box_h
#define Box_h

#include "Geometry/Primitive.h"
#include "Core/Point.h"
class Ray;

class Box : public Primitive {
 public:
  Box(Material* material, const Point& p1, const Point& p2);
  virtual ~Box();

  virtual void transform(const Matrix4 &matrix);
  virtual void getBounds(BoundingBox& bbox) const;
  virtual void intersect(HitRecord& hit, const RenderContext& context, const Ray& ray) const;
  virtual void getNormal(Vector& normal, RenderContext& context,
			 const Point& hitpos, const Ray& ray, const HitRecord& hit) const;
  virtual void computeUVW(Vector& uvw, const RenderContext& context,
			  const Ray& ray, const HitRecord& hit) const;
 protected:
  Point min, max;
};

#endif
