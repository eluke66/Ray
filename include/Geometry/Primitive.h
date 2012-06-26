
#ifndef Primitive_h
#define Primitive_h

#include "stdlib.h"
#include "Geometry/Object.h"
class Material;
class Point;
class Vector;
class Bump;

class Primitive : public Object {
 public:
  Primitive(Material* matl, Bump *bump=NULL);
  virtual ~Primitive();

  void bump( Bump *bump) { _bump = bump; }
  virtual void setMaterial(Material *mat, bool override) {
    if ( !override && matl )
      return;
    matl = mat;
  }
  virtual void preprocess();
  virtual void intersect(HitRecord& hit, const RenderContext& context, const Ray& ray) const = 0;
  void normal(Vector& normal,
	      RenderContext& context,
	      const Point& hitpos,
	      const Ray& ray,
	      const HitRecord& hit) const;

  virtual void computeUVW(Vector& uvw, const RenderContext& context,
                          const Ray& ray, const HitRecord& hit) const;

  Material *getMaterial() { return matl; }
  // TEMP ERIC protected:
  Material* matl;

protected:
  virtual void getNormal(Vector& normal, RenderContext& context,
			 const Point& hitpos, const Ray& ray,
			 const HitRecord& hit) const = 0;
 private:
  Primitive(const Primitive&);
  Primitive& operator=(const Primitive&);

  Bump *_bump;
};


#endif

