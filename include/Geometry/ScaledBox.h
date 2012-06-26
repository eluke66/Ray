
#ifndef ScaledBox_h
#define ScaledBox_h

#include "Geometry/Box.h"

class ScaledBox : public Box {
 public:
  ScaledBox(Material* material, const Point& p1, const Point& p2, double scale=1.0);
  virtual ~ScaledBox();

  virtual void transform(const Matrix4 &matrix);
  virtual void computeUVW(Vector& uvw, const RenderContext& context,
			  const Ray& ray, const HitRecord& hit) const;
 protected:
  double _len;
  double _scale;
  void computeLengths();
};

#endif
