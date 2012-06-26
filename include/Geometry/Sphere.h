
#ifndef Sphere_h
#define Sphere_h

#include "Geometry/Primitive.h"
#include "Core/Point.h"
class Ray;

class Sphere : public Primitive {
 public:
  Sphere(Material* material, const Point& center, double radius);
  virtual ~Sphere();

  virtual void transform(const Matrix4 &matrix);
  virtual void getBounds(BoundingBox& bbox) const;
  virtual void intersect(HitRecord& hit, const RenderContext& context, const Ray& ray) const;
  virtual void getNormal(Vector& normal, RenderContext& context,
			 const Point& hitpos, const Ray& ray, const HitRecord& hit) const;

  void randomPoint( Point &point ) const;
  Vector normal(const Point& hitpos) const;
  const Point& getCenter() const { return center; }
  double getRadius() const { return radius; }
 protected:
  Point center;
  double radius;
  double inv_radius;
};

#endif
