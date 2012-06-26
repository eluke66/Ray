
#ifndef Heightfield_h
#define Heightfield_h

#include "Geometry/Primitive.h"
#include "Core/Point.h"
#include <string>
class Ray;

class Heightfield : public Primitive {
 public:
  Heightfield(Material* material, const std::string& filename,
              const Point& p1, const Point& p2);
  virtual ~Heightfield();

  virtual void transform(const Matrix4 &matrix);
  virtual void getBounds(BoundingBox& bbox) const;
  virtual void intersect(HitRecord& hit, const RenderContext& context, const Ray& ray) const;
  virtual void getNormal(Vector& normal, RenderContext& context,
			 const Point& hitpos, const Ray& ray, const HitRecord& hit) const;

  
  virtual void computeUVW(Vector& uvw, const RenderContext& context,
                          const Ray& ray, const HitRecord& hit) const;
  float getMinimum() const;
protected:
  Point p1, p2;
  float** data;
  int nx, ny;
  Vector diag;
  Vector cellsize;
  Vector inv_cellsize;

  struct HitData {
    int x, y;
  };

  Heightfield(Material *material, const Point &p1, const Point &p2);
  void setData(double minz, double maxz);
};

#endif
