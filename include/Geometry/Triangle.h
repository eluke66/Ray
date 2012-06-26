
#ifndef Triangle_h
#define Triangle_h

#include "Core/Point.h"
#include "Core/Vector.h"
#include "Geometry/Primitive.h"
class HitRecord;
class Point;
class RenderContext;

class Triangle : public Primitive {
 public:
  Triangle(Material* material, const Point& p1, const Point& p2, const Point& p3);
  virtual ~Triangle();

  virtual void transform(const Matrix4 &matrix);
  virtual void getBounds(BoundingBox& bbox) const;
  virtual void intersect(HitRecord& hit, const RenderContext& context, const Ray& ray) const;
  virtual void getNormal(Vector& normal, RenderContext& context,
			 const Point & hitpos, const Ray& ray, const HitRecord& hit) const;

 private:
  Triangle(const Triangle&);
  Triangle& operator=(const Triangle&);

  Point p1, p2, p3;
  Vector e1, e2;
  Vector n;
};

#endif

