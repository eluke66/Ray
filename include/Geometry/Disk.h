
#ifndef Disk_h
#define Disk_h

#include "Core/Vector.h"
#include "Geometry/Primitive.h"
class HitRecord;
class Point;
class RenderContext;

class Disk : public Primitive {
 public:
  Disk(Material* material, const Point& center, const Vector& n, double radius);
  virtual ~Disk();

  virtual void transform(const Matrix4 &matrix);
  virtual void getBounds(BoundingBox& bbox) const;
  virtual void intersect(HitRecord& hit, const RenderContext& context, const Ray& ray) const;
  virtual void getNormal(Vector& normal, RenderContext& context,
			 const Point & hitpos, const Ray& ray, const HitRecord& hit) const;

 private:
  Disk(const Disk&);
  Disk& operator=(const Disk&);

  Point center;
  Vector n;
  double d;
  double radius2;
};

#endif

