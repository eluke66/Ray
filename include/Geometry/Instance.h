
#ifndef Instance_h
#define Instance_h

#include "Geometry/Primitive.h"
#include "Core/Matrix.h"
#include "Core/BoundingBox.h"

class Material;
class Point;
class Vector;

class Instance : public Primitive {
 public:
  Instance(const Matrix4 &mat, Object *object, Material* matl=NULL );
  virtual ~Instance();

  virtual void preprocess();
  virtual void intersect(HitRecord& hit, const RenderContext& context, const Ray& ray) const;
  virtual void getNormal(Vector& normal, RenderContext& context,
			 const Point& hitpos, const Ray& ray, const HitRecord& hit) const;

  virtual void computeUVW(Vector& uvw, const RenderContext& context,
                          const Ray& ray, const HitRecord& hit) const;

  virtual void setMaterial(Material *mat, bool override=true) {
    _matl = mat;
  }
  virtual void transform(const Matrix4 &matrix) {
    _mat = _mat * matrix;
    _mInverse = _mat.invert();
  }
  virtual void getBounds(BoundingBox& bbox) const;

  const Object* instancedObject() const { return _object; }
 protected:
  Matrix4   _mat;
  Matrix4   _mInverse;
  Object *  _object;
  Material* _matl;
  BoundingBox _bbox;
  bool _bboxed;
  
 private:
  Instance(const Instance&);
  Instance& operator=(const Instance&);
  Ray transformRay( const Ray &ray ) const;
};


#endif

