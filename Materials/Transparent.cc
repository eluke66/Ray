#include "Material/TransparentMaterial.h"
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

Transparent::Transparent(double transparency) :
  _transparency(transparency)
{
}


void Transparent::operator()(Color& result, RenderContext &context, const Ray& ray,
				      const HitRecord& hit, const Color& atten, int depth) const
{
  
  // Initial calculations
  const Scene* scene = context.getScene();
  Point hitpos = ray.origin()+ray.direction()*hit.minT();
  Vector normal;
  hit.getPrimitive()->normal(normal, context, hitpos, ray, hit);
  Vector phongnormal = normal;
  double costheta = -Dot(normal, ray.direction());
  bool exiting;
  if(costheta < 0){
    // Exiting surface
    normal = -normal;
    exiting = true;
  } else {
    // Entering surface
    exiting = false;
  }


  // Transparency ray
  Color transp_color;
  Ray transp_ray(hitpos, ray.direction());
  double t_trans = scene->traceRay(transp_color, context, transp_ray, atten*(_transparency), depth+1);
  INC_STAT(RaysTransmitted);
  result = result * (1.0 - _transparency ) + transp_color * _transparency;

}

bool
Transparent::bounce( Ray& newDirection, bool& diffuse,
			      double random, const HitRecord &hit,
			      const Ray &incoming) const
{
  NOT_DONE("Transparent::bounce");
}
