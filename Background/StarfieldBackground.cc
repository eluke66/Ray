
#include "Background/StarfieldBackground.h"
#include "Core/utils.h"
#include "Core/Ray.h"
#include "Core/PerlinNoise.h"

StarfieldBackground::StarfieldBackground(const Color& bgcolor)
  : bgcolor(bgcolor)
{
}

StarfieldBackground::~StarfieldBackground()
{
}

void StarfieldBackground::preprocess()
{
}

void StarfieldBackground::getBackgroundColor(Color& result, const RenderContext&,
                                            const Ray&ray)
{
  result=bgcolor;

#if 0
  double x=(2.0 * drand48())-1.;
  double y = (2.0 * drand48()) - 1.;
  double eps = 1e-1;
  
  if ( ABSCLOSE( ray.direction().x(), x, eps ) &&
       ABSCLOSE( ray.direction().y(), y, eps ) ) {
    double cval = (drand48() / 2.0) + (1./2.);
    result = Color(cval,cval,cval);
  }
#else
  double d = 1 - turbulenceAbs( 2,
				(Point)ray.direction() * 2,
				2.7,
				0.5 );
  d =  d*d*d*d*d*d;
  //if ( d < 0.7 )
  //  d = 0.0;
  double e = 1 - turbulenceAbs( 1,
				(Point)ray.direction() * 2,
				2.7,
				0.5 );
  e =e*e*e*e*e*e;
  //if ( e < 0.7 )
  //  e = 0.0;
  
  d = ( d + e ) / 2.0;
  
  
  double d1 =  2 * turbulenceAbs( 3,
			      (Point)ray.direction() * 255,
			      2.7,
			      0.5 );
  if ( d1 < 1.7 )
    d1 = 0;
  else
    d1 /= 2.0;
  
  double d2 = d / 6 + d1;

  result = Color(d2);
#endif
}
