#ifndef _Utilities_H_
#define _Utilities_H_

#include "Geometry/Primitive.h"
#include "Core/Color.h"
#include "Core/Light.h"
#include "Core/Point.h"
#include "Core/RenderContext.h"
#include "Core/Statistics.h"
#include "Core/HitRecord.h"
#include "Core/Ray.h"
#include "Geometry/Object.h"

#ifndef GLOBAL_ILLUMINATION
#include "Material/FogMaterial.h"
#endif

//#define NO_FALLOFF
#ifndef NO_FALLOFF
#include "Core/DirectionalLight.h"
#endif




class MaterialUtilities {
public:

  template <typename BlurCalc>
  static Color GetBlur( RenderContext &context,
			const Ray &ray,
			const HitRecord &hit,
			const Color &atten,
			int depth,
			BlurCalc calc )
  {
    // Get hitpoint.
    Point hitpos = ray.origin()+ray.direction()*hit.minT();
    Vector normal;
    Color accum;
  
    // Get blur factor and number of samples for hitpoint. 
    double blur;
    int numSamples;
    calc.getBlur( context, ray, hit, blur, numSamples );

    numSamples -= depth*depth*depth;
    if ( numSamples <= 0 )
      numSamples = 1;
    
    // For each sample:
    for ( int i = 0; i < numSamples; ++i ) {
      
      // Get normal.
      hit.getPrimitive()->normal(normal, context, hitpos, ray, hit);
      
      // Perturb normal
      normal.perturb( blur );
    
      // Get and accumulate color
      accum += calc.computeColor( normal, context, ray, hit, atten, depth );
    }
    
    // Normalize and return color
    return accum / (float)numSamples;
  }
  
  template <typename ColorCalc>
  static Color GetLightColor( Light * const * begin,
			      Light * const * end,
			      const RenderContext& context,
			      const Point &hitpos,
			      const Vector &norma,
			      const Object *world,
			      ColorCalc calculator ) {
    Color light;

    Vector normal = norma;
    
    while(begin != end){
      Color thisLight;
      int nSamples = (*begin)->samples();
      int hitShadows = NO_TEST;
      bool allAgree = true;
      for ( int i = 0; i < nSamples; ++i ) {
	Color light_color;
	Vector light_direction;
	double dist = (*begin)->getLight(light_color, light_direction, context, hitpos);
	double cosphi = Dot(normal, light_direction);
	if ( cosphi < 0 ) {
	  normal = -normal;
	  cosphi = Dot(normal, light_direction);
	}
	if(cosphi > 0){
	  INC_STAT(RaysLight);
	  // Cast shadow rays...
	  HitRecord shadowhit(dist);
	  Ray shadowray(hitpos, light_direction);
	  world->intersect(shadowhit, context, shadowray);
	  if(!shadowhit.getPrimitive()
#ifndef GLOBAL_ILLUMINATION
	     || dynamic_cast<_FogMaterial *>(shadowhit.getPrimitive()->matl)
#endif
	     ) {
	    // No shadows...
#ifdef NO_FALLOFF
	    thisLight += calculator(cosphi, light_color, light_direction);
#else
	    if ( dynamic_cast<DirectionalLight *>(*begin) == NULL ) {
	      double dFactor = dist * dist;

	      thisLight +=
		calculator(cosphi, light_color, light_direction) / dFactor;
	    }
	    else
	      thisLight += calculator(cosphi, light_color, light_direction);
#endif
	    if ( hitShadows == NO_TEST ) {
	      hitShadows = NO_SHADOW;
	    }
	    else if ( hitShadows == IN_SHADOW )
	      allAgree = false;
	  }
	  else {
	    if ( hitShadows == NO_TEST )
	      hitShadows = IN_SHADOW;
	    else if ( hitShadows == NO_SHADOW )
	      allAgree = false;
	  }
	} // If we're lighting
	if (  i == nSamples / 2 && allAgree ) {
	  nSamples = i + 1;
	  break;
	}
      } // Samples for light
      thisLight /= (float)nSamples;
      light += thisLight;
      begin++;
    } // This light
    
    return light;
    
  }
private:
  enum { NO_TEST, IN_SHADOW, NO_SHADOW };
  MaterialUtilities();
};


#endif
