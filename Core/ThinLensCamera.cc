
#include "Core/ThinLensCamera.h"
#include "Core/Ray.h"
#include <math.h>

ThinLensCamera::ThinLensCamera(const Point& eye, const Point& lookat,
			       const Vector& up,
			       double hfov,
			       double distance,
			       double aperture,
			       int numSamples)
  : PinholeCamera(eye, lookat, up, hfov ),
    _distance(distance), _lensRadius(aperture/2.0), _numSamples(numSamples)
{
}

ThinLensCamera::~ThinLensCamera()
{
}

void ThinLensCamera::preprocess(double aspect_ratio)
{
  PinholeCamera::preprocess( aspect_ratio );
}

void ThinLensCamera::makeRay(Ray& ray, const RenderContext& context, double x, double y,
			     double a, double b) const
{

  Vector lensU = a*_lensRadius*u;
  Vector lensV = b*_lensRadius*v;
  
  // Normalize
  Vector direction = lookdir+u*x+v*y;
  direction.normalize();

  // Scale by distance
  direction = direction * _distance;

  // And adjust by the lensing factor.
  direction -= lensU + lensV;
  direction.normalize();
  
  // Also tweak the ray origin
  ray = Ray( eye + ( lensU + lensV ),
	     direction );
}
