
#ifndef DielectricMaterial_h
#define DielectricMaterial_h

#include "Material/Material.h"
#include "Core/Color.h"
#include "Core/Scene.h"
#include "Geometry/Primitive.h"
#include "Material/ReflectingDielectric.h"
#include "Material/RefractingDielectric.h"
#include "Material/SolidColor.h"
#include "Material/Utilities.h"
#include "Core/utils.h"

class DielectricLighting {
public:
  DielectricLighting(int phong_exponent) : _phong_exponent(phong_exponent) {}
  void operator()(Color& result, RenderContext& context, const Ray& ray,
		  const HitRecord& hit, const Color& atten, int depth) const
  {
    // Initial calculations
    const Scene* scene = context.getScene();
    const std::vector<Light*>& lights = scene->getLights();
    Point hitpos = ray.origin()+ray.direction()*hit.minT();
    Vector normal;
    hit.getPrimitive()->normal(normal, context, hitpos, ray, hit);
    double costheta = Dot(normal, ray.direction());

    Vector dir(ray.direction());
    if ( costheta < 0.0 )
      dir = -dir;
    
    // Calculate lighting.
    const Object* world = scene->getObject();
    Light*const* begin = &lights[0];
    Light*const* end = &lights[lights.size()];
    
    ReflectingShader s(dir,
		       normal,
		       _phong_exponent );
    result += MaterialUtilities::GetLightColor( begin, end, context,
						hitpos, normal, world, s );
  }
  bool bounce( Ray& newDirection, bool& diffuse, double random,
	       const HitRecord &hit, const Ray &incoming) const {
    // For dielectric materials, the dielectric itself will determine
    // reflection/refraction. We neither bounce nor store!
    diffuse = false;
    return false;
  }

private:
  int _phong_exponent;
};


typedef _Material<SolidColor,DielectricLighting,RefractingDielectric> _DielectricMaterial;
#define DielectricMaterial(a,b,c) \
  _DielectricMaterial(SolidColor(b), DielectricLighting(c), RefractingDielectric(a))

#define GlossyDielectricMaterial(a,b,c,d) \
  _DielectricMaterial(SolidColor(b.log()), DielectricLighting(c), RefractingDielectric(a,d))
#endif
