#ifndef CloudColor_h
#define CloudColor_h

#include "Core/Color.h"
class Ray;
class RenderContext;

#include "Core/HitRecord.h"
#include "Core/Vector.h"
#include "Geometry/Primitive.h"
#include "Core/PerlinNoise.h"
#include "Core/utils.h"


template <typename BGType, typename CloudType>
class CloudColor {
public:
  CloudColor(const BGType& background, const CloudType& cloud,
	     double cloudiness) : 
    _bg(background), _cloud(cloud), _cloudiness(cloudiness)
  {
    assert( cloudiness <= 1.0 && cloudiness >= 0.0 );
  }

  void operator()(Color& result, const Vector &uvw ) const
  {
    // Do we need more of these summed?!
    double pval = (turbulence( 4,
			       (Point)uvw* 512,
			       0.5, 2.0 ) + 10) / 20.0;
    pval = clamp(pval);
    double c = pval - _cloudiness;
    c = clamp(c);
    
    double CloudSharpness = 0.35;
    
    // This is the proportion of cloudiness. 0 = empty sky
    double factor = clamp(::pow( CloudSharpness, c));
    double cloudDensity = 1.0 - factor;
    
    Color cloudColor, bgColor;
    _cloud(cloudColor, uvw );
    _bg(bgColor, uvw );
    
    result = cloudColor * cloudDensity + bgColor * (1.0 - cloudDensity);
  }
  
  void operator()(Color& result, RenderContext& context, const Ray& ray,
		  const HitRecord& hit, const Color& atten, int depth) const
  {
    Vector uvw;
  hit.getPrimitive()->computeUVW(uvw, context, ray, hit);

  // Do we need more of these summed?!
  double pval = (turbulence( 4,
			     (Point)uvw* 512,
			     0.5, 2.0 ) + 10) / 20.0;
  pval = clamp(pval);
  double c = pval - _cloudiness;
  c = clamp(c);

  double CloudSharpness = 0.35;

  // This is the proportion of cloudiness. 0 = empty sky
  double factor = clamp(::pow( CloudSharpness, c));
  double cloudDensity = 1.0 - factor;

  Color cloudColor, bgColor;
  _cloud(cloudColor, context, ray, hit, atten, depth);
  _bg(bgColor, context, ray, hit, atten, depth);
  
  result = cloudColor * cloudDensity + bgColor * (1.0 - cloudDensity);
  }
  
private:
  BGType _bg;
  CloudType _cloud;
  double _cloudiness;
};

#endif
