#include "Material/Material.h"
#include "Material/CheckerColor.h"

#include "Core/HitRecord.h"
#include "Core/Vector.h"
#include "Geometry/Primitive.h"


CheckerColor::CheckerColor(const Point& origin, const Vector& v1, const Vector& v2,
			   Material* matl0, Material* matl1)
  : _origin(origin), _v1(v1), _v2(v2), _v3(0,0,0)
{
  _v1 *= 1./v1.length2();
  _v2 *= 1./v2.length2();
  _matls[0] = matl1;
  _matls[1] = matl0;
  _matls[2] = matl1;
}

CheckerColor::CheckerColor(const Point& origin,
			   const Vector& v1, const Vector& v2, const Vector& v3,
			   Material* matl0, Material* matl1)
  : _origin(origin), _v1(v1), _v2(v2), _v3(v3)
{
  _v1 *= 1./v1.length2();
  _v2 *= 1./v2.length2();
  _v3 *= 1./v3.length2();
  _matls[0] = matl0;
  _matls[1] = matl1;
  _matls[2] = matl0;
}

CheckerColor::CheckerColor(const CheckerColor&c) :
    _origin(c._origin), _v1(c._v1), _v2(c._v2), _v3(c._v3) {
    _matls[0] = c._matls[0];
    _matls[1] = c._matls[1];
    _matls[2] = c._matls[2];
  }

void CheckerColor::operator()(Color& result, RenderContext& context,
			      const Ray& ray, const HitRecord& hit,
			      const Color& atten, int depth) const
{
  Vector uvw;
  hit.getPrimitive()->computeUVW(uvw, context, ray, hit);
  int i1 = Floor(Dot(uvw, _v1));
  int i2 = Floor(Dot(uvw, _v2));
  int i3 = Floor(Dot(uvw, _v3));
  int which_matl = (i1+i2+i3)%2+1;
  _matls[which_matl]->shade(result, context, ray, hit, atten, depth);
}
