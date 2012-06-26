#include "Material/WoodColor.h"

#include "Core/HitRecord.h"
#include "Core/Vector.h"
#include "Geometry/Primitive.h"
#include "Core/PerlinNoise.h"



void
WoodColor::operator()(Color& result, RenderContext& context,
		      const Ray& ray,
		      const HitRecord& hit, const Color&, int) const {
  Vector uvw;
  hit.getPrimitive()->computeUVW(uvw, context, ray, hit);
  double px = uvw.x()*_scale;
  double py = uvw.y()*_scale;
  double pz = uvw.z()*_scale;
  double cx = _cscale*perlinNoise(Point(px*_cfreq, py*_cfreq, pz*_cfreq));
  double cy = _cscale*perlinNoise(Point(px*_cfreq+1, py*_cfreq+1, pz*_cfreq+1));
  px -= cx;
  py -= cy;
  float r = sqrt(px*px+py*py)+
    _rscale*turbulence(_octaves,
			Point(px*_rfreq, py*_rfreq, pz*_rfreq),
			_lacunarity,
			_gain);
  double value = cos(r)*0.5+0.5;
  value = ipow(value, _expon);
  result = _c1*(1-value)+_c2*value;
}

