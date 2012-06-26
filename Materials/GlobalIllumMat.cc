#include "Material/GlobalIllumMat.h"
#include "Core/Color.h"
#include "Core/RenderContext.h"
#include "Core/Ray.h"
#include "Core/Scene.h"
#include "Core/HitRecord.h"
#include "Core/utils.h"
#include "GlobalIllum/PhotonMap.h"
#include "Geometry/Primitive.h"

// Accurate:
// Color = diffuse * illum
// illum = samples / numSamples
// sample = 

Color
accurateSolution( RenderContext &context,
		  const Point &hitpos,
		  const Vector &normal,
		  int depth )
		  
{
  // Sample M x N items above the surface.
  int M = 5;
  int N = 5;
  const Scene * scene = context.getScene();
  assert(scene);

  Color totalSum(0.0);
  
  for ( int k = 0; k < M; ++k )
    for ( int j = 0; j < N; ++j ) {

      // Get jittered sample
      double Xj = (j+drand48()) / N;
      double Yk = (k+drand48()) / M;

      double phi = 2*M_PI*Yk;
      double theta = acos(sqrt(1.0 - Xj));

      Color result;
      Ray  ray( hitpos,
	        Vector( normal.x()*cos(phi)*sin(theta),
			normal.y()*sin(phi)*sin(theta),
			normal.z()*cos(theta) ) );
      scene->traceRay( result, context, ray, Color(1.0), depth+1 );
      
      totalSum += result;
    }
  
  return totalSum * (1.0 / (M*N));
}

void
GlobalIllumMat::operator()(Color& result,RenderContext& context,
			   const Ray& ray, const HitRecord& hit,
			   const Color& atten, int depth) const

{
  // Grab indirect (and caustic) lighting from the scene's photon
  // map(s).
  const Scene * scene = context.getScene();
  assert(scene);
  const Primitive *prim = hit.getPrimitive();
  if ( prim == NULL )
    return;
  
  Point pos( ray.origin() + hit.minT() * ray.direction() );
  Vector normal;
  prim->normal( normal, context, pos, ray, hit );
  
  // Indirect
  if ( Options::Instance()->numPhotons() > 0 ) {
    PhotonMap *map = scene->getPhotonMap(true);
    assert(map);
    Color global;
    // Get indirect, non-caustic lighting
    if ( depth < 0 )
      global = accurateSolution( context, pos, normal, depth ) * 0.6;
    else
      map->getIrradience( global,
			  pos,
			  normal,
			  Options::Instance()->estimateRadius(),
			  Options::Instance()->photonsInEstimate() );
    result += global*context.pigment;
  }

  // Get caustic lighting
  if ( Options::Instance()->numCausticPhotons() > 0 ) {
    PhotonMap *cmap = scene->getCausticPhotonMap(true);
    assert(cmap);
    Color caustic;
    cmap->getIrradience( caustic,
			 pos,
			 normal,
			 Options::Instance()->causticEstimateRadius(),
			 Options::Instance()->photonsInCausticEstimate());
    result += caustic*context.pigment;
  }
}
