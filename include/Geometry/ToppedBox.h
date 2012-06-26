
#ifndef ToppedBox_h
#define ToppedBox_h

#include "Geometry/Box.h"

class ToppedBox : public Box {
 public:
  ToppedBox(Material* boxMaterial, Material *topMaterial, const Point& p1, const Point& p2);
  virtual ~ToppedBox();

  virtual void intersect(HitRecord& hit, const RenderContext& context, const Ray& ray) const;
 protected:
  Material *_topMaterial;
};

#endif
