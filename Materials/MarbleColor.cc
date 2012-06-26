#include "Material/MarbleColor.h"
#include "Core/HitRecord.h"
#include "Core/Vector.h"
#include "Geometry/Primitive.h"
#include "Core/PerlinNoise.h"


void MarbleColor::operator()(Color& result, RenderContext& context, const Ray& ray,
			      const HitRecord& hit, const Color& atten, int depth) const {

  Vector uvw;
  hit.getPrimitive()->computeUVW(uvw, context, ray, hit);
  Point texturep(uvw*_scale*_fscale);
  double value = cos(uvw.x()*_scale+_tscale*turbulenceAbs(_octaves, texturep, _lacunarity, _gain));
  value = value*0.5+0.5;
  result = _c1*(1-value)+_c2*value;
}
