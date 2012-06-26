#include "Material/ReflectingDielectric.h"


#include "Core/Ray.h"
#include "Core/HitRecord.h"
#include "Core/Scene.h"
#include "Core/RenderContext.h"
#include "Geometry/Primitive.h"
#include "Material/Blur.h"
#include "Material/Utilities.h"
#include "Core/utils.h"

void
ReflectingDielectric::getBlur( RenderContext &context,
			       const Ray &ray,
			       const HitRecord &hit,
			       double &blur,
			       int &numSamples ) const
{
  assert( _blur );
  
  blur = _blur->getBlur( context, ray, hit );
  numSamples = 10; // TEMP!
}

Color 
ReflectingDielectric::computeColor( const Vector &norm, RenderContext &context,
				    const Ray& ray,
				    const HitRecord& hit,
				    const Color& atten, int depth) const {
  Color refl = context.originalColor;
  Color not_refl = Color(1,1,1) - refl;
  const Scene* scene = context.getScene();
  
  Point hitpos = ray.origin()+ray.direction()*hit.minT();
  Vector normal = norm;
  
  double costheta = Dot(normal, ray.direction());
  if(costheta > 0){
    normal = -normal;
  } else {
    costheta = -costheta;
  }
  
  // Calculate reflected ray
  double k = 1-costheta;
  double k2 = k*k;
  double k4 = k2*k2;
  double k5 = k4*k;
  Color R = (refl + not_refl*k5);
  Vector refl_dir = ray.direction()+2*costheta*normal;
  Color refl_color;
  Ray refl_ray(hitpos, refl_dir);
  
  // Trace ray with custom parameters
  scene->traceRay(refl_color, context, refl_ray, atten*R, depth+1);
  INC_STAT(RaysReflected);
  
  // Scale reflected color
  return R * refl_color * _reflectivity + ( refl * (1.0-_reflectivity) );
}
  
void 
ReflectingDielectric::operator()(Color& result,
				 RenderContext &context, const Ray& ray,
				 const HitRecord& hit, const Color& atten,
				 int depth) const
{
  context.originalColor = result;
  if ( _blur ) {
     result = MaterialUtilities::GetBlur( context, ray, hit, atten, depth, *this );
  }
  else {
    Vector normal;
    Point hitpos = ray.origin()+ray.direction()*hit.minT();
    hit.getPrimitive()->normal(normal, context, hitpos, ray, hit);
    result = computeColor( normal, context, ray, hit, atten, depth );
  }
}


bool
ReflectingDielectric::bounce( Ray& newDirection, bool& diffuse,
			      double random, const HitRecord &hit,
			      const Ray &incoming) const
{
  RenderContext context(NULL);
  diffuse = false; // Never store photons

  if ( _blur ) {
    NOT_DONE("ReflectingDielectric::bounce with blur");
  }
  else {
    Vector normal;
    Point hitpos = incoming.origin()+incoming.direction()*hit.minT();
    hit.getPrimitive()->normal(normal, context, hitpos, incoming, hit);

    double costheta = Dot(normal, incoming.direction());
    if(costheta > 0){
      normal = -normal;
    } else {
      costheta = -costheta;
    }
    Vector refl_dir = incoming.direction()+2.0*costheta*normal;
    newDirection.set( hitpos, refl_dir );
  }
  return ( random <= _reflectivity );
}
