#ifndef CheckerColor_h
#define CheckerColor_h

#include "Core/Color.h"
#include "Core/Point.h"
#include "Core/Vector.h"

class Material;
class HitRecord;
class Ray;
class RenderContext;

class CheckerColor {
public:
  CheckerColor(const Point& origin, const Vector& v1, const Vector& v2,
	       Material* matl0, Material* matl1);
  CheckerColor(const Point& origin, const Vector& v1, const Vector& v2, const Vector& v3,
	       Material* matl0, Material* matl1);
  void operator()(Color& result, RenderContext& context, const Ray& ray,
		  const HitRecord& hit, const Color& atten, int depth) const;

  
  CheckerColor(const CheckerColor&c); 
    
private:
  Point _origin;
  Vector _v1, _v2, _v3;
  Material* _matls[3];

  CheckerColor();
  CheckerColor &operator=(const CheckerColor&);
};

#endif
