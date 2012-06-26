#ifndef TransparentMaterial_h
#define TransparentMaterial_h

#include "Material/Material.h"
#include "Core/Color.h"

#include "Material/SolidColor.h"
#include "Material/Utilities.h"
class Color;
class RenderContext;
class Ray;
class HitRecord;

class Transparent {
public:
  Transparent(double transparency);
  
  void operator()(Color& result, RenderContext &context, const Ray& ray,
		  const HitRecord& hit, const Color& atten, int depth) const;
  bool bounce( Ray& newDirection, bool& diffuse, double random, const HitRecord &hit, const Ray &incoming) const;
private:
  double _transparency;
};

typedef _Material<SolidColor,NoOp, Transparent> _TransparentMaterial;
#define TransparentMaterial(a,b) \
  _TransparentMaterial(SolidColor(a), NoOp(), Transparent(b) )
#endif
