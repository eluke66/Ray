
#ifndef Mountains_h
#define Mountains_h

#include "Geometry/Primitive.h"
#include "Core/Point.h"
class Ray;
class Material;
class Group;
class MtnFunc;

class Mountains : public Primitive {
public:
  Mountains(Material* matl, const Point& p1, const Point& p2,
	    double xOffset=0.0, double yOffset=0.0, bool falloff=true);
  virtual ~Mountains();
  virtual void setMaterial(Material *mat, bool override) {
    Primitive::setMaterial(mat, override);
    if ( matl == mat )
      _geom->setMaterial(mat,override);
  }
  virtual void transform(const Matrix4 &matrix);
  virtual void getBounds(BoundingBox& bbox) const;
  virtual void intersect(HitRecord& hit, const RenderContext& context, const Ray& ray) const;
  virtual void getNormal(Vector& normal, RenderContext& context,
			 const Point& hitpos, const Ray& ray, const HitRecord& hit) const;
  virtual void computeUVW(Vector& uvw, const RenderContext& context,
			  const Ray& ray, const HitRecord& hit) const;

  double valueAt( double x, double y ) const;
 private:
  Primitive * _geom;
  MtnFunc   * _func;
};

#endif
