#include <stdlib.h>
#include "Material/RefractingDielectric.h"
#include "Core/Color.h"
#include "Core/RenderContext.h"
#include "Core/Ray.h"
#include "Core/Point.h"
#include "Core/Vector.h"
#include "Core/Statistics.h"
#include "Core/HitRecord.h"
#include "Geometry/Primitive.h"
#include "Core/Scene.h"

#include "Core/utils.h"
#include "Material/Blur.h"
#include "Material/Utilities.h"

RefractingDielectric::RefractingDielectric(double eta, Blur *blur) :
  _eta(eta), _blur(blur)
{
  _inv_eta = 1. / _eta;
  _r0 = (_eta-1)/(_eta+1);
  _r0 = _r0*_r0;
}

void
RefractingDielectric::getBlur( RenderContext &context,
			       const Ray &ray,
			       const HitRecord &hit,
			       double &blur,
			       int &numSamples ) const {
  assert( _blur );
  
  blur = _blur->getBlur( context, ray, hit );
  numSamples = 5; // TEMP!
}


Color  
RefractingDielectric::computeColor( const Vector &norm,
				    RenderContext &context,
				    const Ray& ray,
				    const HitRecord& hit,
				    const Color& atten, int depth) const {

  // Initial calculations
  const Scene* scene = context.getScene();
  Point hitpos = ray.origin()+ray.direction()*hit.minT();
  Color result;

  Vector normal = norm;
  Vector phongnormal = normal;
  double costheta = -Dot(normal, ray.direction());
  double eta_inverse;
  bool exiting;
  if(costheta < 0){
    // Exiting surface
    normal = -normal;
    costheta = -costheta;
    eta_inverse = _eta;
    exiting = true;
  } else {
    // Entering surface
    eta_inverse = _inv_eta;
    exiting = false;
  }


  // Calculate reflected ray
  Vector refl_dir = ray.direction()+2*costheta*normal;
  double costheta2squared =
    1 + (costheta*costheta-1)*(eta_inverse*eta_inverse);
  if(costheta2squared < 0){
    // Total internal reflection...

    // Trace ray with custom parameters
    Ray refl_ray(hitpos, refl_dir);
    double t_refl;
    Color refl_color;
    INC_STAT(RaysReflected);
    t_refl = scene->traceRay(refl_color, context, refl_ray, atten, depth+1);
    

    // Scale reflected color
    if ( exiting ) {
      Color attenuation(exp(result.r()*t_refl),
			exp(result.g()*t_refl),
                        exp(result.g()*t_refl));
      result = refl_color * attenuation;
    } else {
      result = refl_color;
    }

  } else {
    double costheta2 = sqrt(costheta2squared);
    double k = 1-Min(costheta, costheta2);
    double k2 = k*k;
    double k4 = k2*k2;
    double k5 = k4*k;
    double refl = _r0*(1-k5)+k5;

    // Reflective ray
    // Trace ray with custom parameters
    Color refl_color;
    Ray refl_ray(hitpos, refl_dir);
    double t_refl = scene->traceRay(refl_color, context, refl_ray, atten*refl, depth+1);
    INC_STAT(RaysReflected);

    // Scale reflected color
    result = refl_color*refl;
    
    // Transparency ray
    Vector transp_dir = eta_inverse*ray.direction()+(eta_inverse*costheta-costheta2)*normal;
    Color transp_color;
    Ray transp_ray(hitpos, transp_dir);
    double t_trans = scene->traceRay(transp_color, context, transp_ray, atten*(1-refl), depth+1);
    INC_STAT(RaysTransmitted);
    if(!exiting){
      Color attenuation(1-exp(-context.originalColor.r()*t_trans),
			1-exp(-context.originalColor.g()*t_trans),
                        1-exp(-context.originalColor.b()*t_trans));
      result += transp_color * attenuation*(1-refl);
    } else {
      result += transp_color*(1-refl);
    }
  }
  return result;
}



void RefractingDielectric::operator()(Color& result, RenderContext &context, const Ray& ray,
				      const HitRecord& hit, const Color& atten, int depth) const
{
  if ( _blur ) {
    result = MaterialUtilities::GetBlur( context, ray, hit,
					 atten, depth, *this );
  }
  else {
    Vector normal;
    Point hitpos = ray.origin()+ray.direction()*hit.minT();
    hit.getPrimitive()->normal(normal, context, hitpos, ray, hit);
    context.originalColor = result;
    result = computeColor( normal, context, ray, hit, atten, depth );
  }
}




bool RefractingDielectric::bounce( Ray& newDirection, bool& diffuse,
				   double random, const HitRecord &hit,
				   const Ray &incoming) const
{
  diffuse = false; // Not a diffuse surface
  RenderContext context(NULL);
  
  // Initial calculations
  Point hitpos = incoming.origin()+incoming.direction()*hit.minT();
  Vector normal;
  hit.getPrimitive()->normal(normal, context, hitpos, incoming, hit);
  Vector phongnormal = normal;
  double costheta = -Dot(normal, incoming.direction());
  double eta_inverse;
  bool exiting;
  if(costheta < 0){
    // Exiting surface
    normal = -normal;
    costheta = -costheta;
    eta_inverse = _eta;
    exiting = true;
  } else {
    // Entering surface
    eta_inverse = _inv_eta;
    exiting = false;
  }


  // Calculate reflected ray
  Vector refl_dir = incoming.direction()+2*costheta*normal;
  double costheta2squared =
    1 + (costheta*costheta-1)*(eta_inverse*eta_inverse);
  if(costheta2squared < 0){

    // Total internal reflection - a single reflected ray.
    newDirection.set( hitpos, refl_dir );

  } else {
    double costheta2 = sqrt(costheta2squared);
    double k = 1-Min(costheta, costheta2);
    double k2 = k*k;
    double k4 = k2*k2;
    double k5 = k4*k;
    double refl = _r0*(1-k5)+k5;

    // Russian roulette - if reflection probability is higher than the
    // random number, we reflect. Else, we refract.
    if ( refl > random ) {
      // Reflect
      newDirection.set(hitpos, refl_dir);
    }
    else {
      
      // Refract
      Vector transp_dir = eta_inverse*incoming.direction()+(eta_inverse*costheta-costheta2)*normal;

      
      newDirection.set( hitpos, transp_dir );
    }
  }

  return true;
}
