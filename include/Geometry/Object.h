
#ifndef Object_h
#define Object_h

#include <assert.h>
#include "Core/Statistics.h"

class BoundingBox;
class Color;
class HitRecord;
class Ray;
class RenderContext;
class Matrix4;
class Material;

class Object {
 public:
  Object();
  virtual ~Object();

  virtual void setMaterial(Material *mat, bool override=true) = 0;
  virtual void preprocess();
  virtual void transform(const Matrix4 &matrix) = 0;
  virtual void getBounds(BoundingBox& bbox) const = 0;
  virtual void intersect(HitRecord& hit, const RenderContext& context, const Ray& ray) const = 0;

 private:
  Object(const Object&);
  Object& operator=(const Object&);
};

#endif

