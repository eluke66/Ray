
#ifndef Plane_h
#define Plane_h

#include "Core/Vector.h"
#include "Geometry/Primitive.h"
class HitRecord;
class Point;
class RenderContext;

class Plane : public Primitive {
 public:
  Plane(Material* material, const Vector& n, const Point& point);
  virtual ~Plane();

  virtual void transform(const Matrix4 &matrix);
  virtual void getBounds(BoundingBox& bbox) const;
  virtual void intersect(HitRecord& hit, const RenderContext& context, const Ray& ray) const;
  virtual void getNormal(Vector& normal, RenderContext& context,
			 const Point & hitpos, const Ray& ray, const HitRecord& hit) const;
  virtual void computeUVW(Vector& uvw, const RenderContext& context,
                          const Ray& ray, const HitRecord& hit) const;

  const Vector &normal() const { return n; }
  virtual void randomPoint(Point &pt) const;
  
 protected:
  Plane(const Plane&);
  Plane& operator=(const Plane&);

  Vector n;
  Point p;
  double d;
};

#endif

