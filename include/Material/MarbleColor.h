#ifndef MarbleColor_h
#define MarbleColor_h

#include "Core/Color.h"
class HitRecord;
class Ray;
class RenderContext;


class MarbleColor {
public:
  MarbleColor(const Color& c1, const Color& c2,
	      double scale, int octaves, double tscale, double fscale,
	      double lacunarity, double gain ) : 
    _c1(c1),_c2(c2),_scale(scale),_octaves(octaves),_tscale(tscale),
    _fscale(fscale), _lacunarity(lacunarity),_gain(gain) {}

  void operator()(Color& result, RenderContext& context, const Ray& ray,
		  const HitRecord& hit, const Color& atten, int depth) const;
private:
  Color _c1, _c2;
  double _scale;
  int _octaves;
  double _tscale;
  double _fscale;
  double _lacunarity;
  double _gain;
};

#endif
