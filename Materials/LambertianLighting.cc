#include "Material/LambertianLighting.h"

#include "Geometry/Primitive.h"
#include "Core/Scene.h"
#include "Core/Ray.h"
#include "Core/Light.h"
#include "Material/Utilities.h"

struct LambertianShader {
public:
  LambertianShader( double Kd ) : _Kd(Kd) {}
  inline Color operator()( double cosphi, const Color& light_color, const Vector &light_direction ) {
    return light_color*(_Kd*cosphi);
  }

private:
  double _Kd;
};


void LambertianLighting::operator()(Color& result, RenderContext& context, const Ray& ray,
				    const HitRecord& hit, const Color& atten, int depth) const
{
  
  const Scene* scene = context.getScene();
  const std::vector<Light*>& lights = scene->getLights();
  Point hitpos = ray.origin()+ray.direction()*hit.minT();
  Vector normal;
  hit.getPrimitive()->normal(normal, context, hitpos, ray, hit);
  double costheta = Dot(normal, ray.direction());
  if(costheta > 0)
    normal = -normal;

  const Object* world = scene->getObject();

  Color light = scene->getAmbient()*_Ka;

  Light*const* begin = &lights[0];
  Light*const* end = &lights[lights.size()];

  light += MaterialUtilities::GetLightColor( begin, end, context,
					     hitpos, normal, world,
					     LambertianShader(_Kd) );
  result = light*result;
}

bool
LambertianLighting::bounce( Ray& newDirection, bool& diffuse,
			    double random, const HitRecord &hit,
			    const Ray &incoming) const
{
  RenderContext context(NULL);

  // Find the normal
  Point hitpos = incoming.origin()+incoming.direction()*hit.minT();
  Vector normal;
  hit.getPrimitive()->normal(normal, context, hitpos, incoming, hit);

  newDirection.set( hitpos,
		    Vector::CosineWeightedVector( normal ) );
  
  diffuse = true; // Store a photon

  return true;
}
