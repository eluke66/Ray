#ifndef WoodColor_h
#define WoodColor_h

#include "Core/Color.h"

class HitRecord;
class Ray;
class RenderContext;

class WoodColor {
public:
  WoodColor(const Color& c1, const Color& c2,
	    double scale, int octaves, double rscale, double rfreq,
	    double cscale, double cfreq,
	    double lacunarity, double gain, int expon) :
    _c1(c1),_c2(c2),_scale(scale),_octaves(octaves),_rscale(rscale),
    _rfreq(rfreq),_cscale(cscale), _cfreq(cfreq),
    _lacunarity(lacunarity),_gain(gain), _expon(expon) {}
  
  void operator()(Color& result, RenderContext& context, const Ray& ray,
		  const HitRecord& hit, const Color& atten, int depth) const;

private:
  Color _c1, _c2;
  double _scale;
  int _octaves;
  double _rscale;
  double _rfreq;
  double _cscale;
  double _cfreq;
  double _lacunarity;
  double _gain;
  int _expon;
};


#endif
