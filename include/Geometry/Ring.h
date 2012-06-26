
#ifndef Ring_h
#define Ring_h

#include "Core/Vector.h"
#include "Geometry/Primitive.h"
class HitRecord;
class Point;
class RenderContext;

class Ring : public Primitive {
 public:
  Ring(Material* material, const Point& center, const Vector& n, double radius1, double radius2);
  virtual ~Ring();

  virtual void transform(const Matrix4 &matrix);
  virtual void getBounds(BoundingBox& bbox) const;
  virtual void intersect(HitRecord& hit, const RenderContext& context, const Ray& ray) const;
  virtual void getNormal(Vector& normal, RenderContext& context,
			 const Point & hitpos, const Ray& ray, const HitRecord& hit) const;

 private:
  Ring(const Ring&);
  Ring& operator=(const Ring &);

  Point center;
  Vector n;
  double d;
  double inner_radius2;
  double outer_radius2;
};

#endif

