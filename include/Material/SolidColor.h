#ifndef _SolidColor_h
#define _SolidColor_h

#include "Core/Color.h"
class HitRecord;
class Ray;
class RenderContext;
class Vector;

class SolidColor {
public:
  SolidColor(const Color &color) : _color(color) {}
  void operator()(Color& result, RenderContext& context, const Ray& ray,
		  const HitRecord& hit, const Color& atten, int depth) const {
    result = _color;
  }
  void operator()(Color& result, const Vector &uvw) const {
    result = _color;
  }
private:
  const Color _color;
  SolidColor();
};

#endif
