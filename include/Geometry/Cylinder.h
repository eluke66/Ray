
#ifndef Cylinder_h
#define Cylinder_h

#include "Core/Vector.h"
#include "Geometry/Primitive.h"
#include "Core/Matrix.h"

class HitRecord;
class Point;
class RenderContext;

class Cylinder : public Primitive {
 public:
  Cylinder(Material* material, double height, double radius );
  virtual ~Cylinder();

  virtual void transform(const Matrix4 &matrix);
  virtual void getBounds(BoundingBox& bbox) const;
  virtual void intersect(HitRecord& hit, const RenderContext& context, const Ray& ray) const;
  virtual void getNormal(Vector& normal, RenderContext& context,
			 const Point & hitpos, const Ray& ray, const HitRecord& hit) const;

 private:
  Cylinder(const Cylinder&);
  Cylinder& operator=(const Cylinder &);
  void intersectI(HitRecord& hit, const RenderContext& context, const Ray& ray) const;
  void normalI(Vector& normal, RenderContext& context,
	       const Point & hitpos, const Ray& ray, const HitRecord& hit) const;
  
  double _height;
  double _radius;
  Matrix4 _matrix, _mInverse;
};

#endif

