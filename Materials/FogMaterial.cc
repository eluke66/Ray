#include "Material/FogMaterial.h"
#include "Core/Point.h"
#include "Core/Scene.h"
#include "Core/RenderContext.h"
#include "Core/Ray.h"
#include "Core/HitRecord.h"
#include "Core/utils.h"
#include "Core/PerlinNoise.h"
#include "Core/BoundingBox.h"
#include "Geometry/Primitive.h"

/*
  For fog generally:
  1. Number of sample points.
  2. Opacity scale.
  3. Fog color.
  4. Fog bounds

  To calculate fog, we need functors:
  0. Constant
  1. 3D Perlin - standard perlin inputs.
  2. Linear gradient - up to 3 vectors for falloff, plus boundary
  conds for each.
  3. Exponential gradient - Same as linear, but also with k value.
  
  4. Gaussian falloff - up to 3 vectors for falloff, plus stddev.
*/

double
PerlinFogCalculator::valueAt( const Point &point,
			      const BoundingBox &b ) const {
  
  return turbulence(_octaves, (Point)(point - b.getMin()), _lacunarity, _gain);
}

double
GradientFogCalculator::findGradientSpot(const Point &point,
					const BoundingBox &b ) const {
  // 1 at start of vector, 0 at end
  
  // Find min corner of bbox (depends on vector sign)
  Point min = b.getMin();
  Point max = b.getMax();

#ifdef ORIG
  Point minCorner( (_falloff.x() < 0) ? min.x() : max.x(),
		   (_falloff.y() < 0) ? min.y() : max.y(),
		   (_falloff.z() < 0) ? min.z() : max.z() );
#else
  Point minCorner( min );
  Point maxCorner( max );
#endif
  
  // Create a vector from min to point.
  Vector pointV = (point - minCorner) / (maxCorner-minCorner);
  //pointV.normalize();

  // Dot falloff and point, subtract from 1 (as farther = less
  // opaque).
  return 1.-Dot( pointV, _falloff );
}

double
LinearFogCalculator::valueAt( const Point &point,
			      const BoundingBox &b ) const {
  
  double proportion = findGradientSpot(point,b);

  // Linear slope from start to end.
  return proportion * _start + ( 1.0 - proportion ) * _end;
}

double
ExpFogCalculator::valueAt( const Point &point,
			   const BoundingBox &b ) const {

  // Get the distance
  double proportion = findGradientSpot(point,b);

  // Find the true decay
  double decay = exp( _falloffK * (1.0 - proportion) );
  return decay * _start + ( 1.0 - decay ) * _end;
}

double
GaussianFogCalculator::valueAt( const Point &point,
				const BoundingBox &b ) const {
  NOT_DONE("GaussianFogCalculator::valueAt");
  return 0;
}


FogMaterial::FogMaterial(double opacityScale, const Color &color,
			 int numSamples,
			 FogCalculator *calc) :
  _scale(opacityScale / (double)numSamples), _color(color), _numSamples(numSamples),
  _calc(calc)
{

}

FogMaterial::~FogMaterial() {

}

void valuesAt( Color &color, double &opacity, const Point &point) {
  double v = point.z() / 120.0;
  color = Color(1.0);
  opacity = v;
}

/*
  - Entry into fog
  - Object color in fog, or object color from fog exit ray.
  - 
   


*/



void
FogMaterial::operator()(Color& result,
			RenderContext& context,
			const Ray& ray,
			const HitRecord& hit,
			const Color& atten,
			int depth) const {

  // Find the exit ray, and intersect the scene.
  const Scene* scene = context.getScene();
  Point hitpos = ray.origin()+ray.direction()*hit.minT();

  Point exitPoint = hitpos + ray.direction()*1e-6;
  Ray exitray(exitPoint, ray.direction());

  Color exitColor;
  int i;
  // From the start hitpoint to the end hitpoint, measure the distance.
  double dist = scene->traceRay( exitColor,
				 context,
				 exitray,
				 atten,
				 depth+1 );
  Color accum_color(0.0);
  double accum_opacity = 0.0;
  
  // If no hit, just return exit color.
  if ( dist == DBL_MAX ) {
  }
  else {
    BoundingBox b;
    hit.getPrimitive()->getBounds(b);
    
    // Ray March => iterate N times:
    double distFactor = (dist / (double)_numSamples);
    Vector stepSize = ray.direction() * distFactor;
    Point evalPoint = hitpos;
    for ( i = 0; (i < _numSamples) && (accum_opacity < 0.95); ++i ) {
      
      // Accum opacity.
      double opacity = _calc->valueAt(evalPoint,b) * _scale * distFactor;
      if ( opacity > 0.0 ) {

	// Break if at full opacity.
	accum_color += _color * opacity * (1-accum_opacity);
	accum_opacity += opacity * (1-accum_opacity);
      }
      
      // Inc start point of test by dist/N.
      evalPoint += stepSize;
    }
  }

  // Color of ray is then fog * opacity + final * (1-opacity)
  result = accum_color + exitColor * (1 - accum_opacity);
}
