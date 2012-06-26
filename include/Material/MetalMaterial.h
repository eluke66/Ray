
#ifndef MetalMaterial_h
#define MetalMaterial_h

#include "Material/Material.h"
#include "Core/Color.h"
#include "Material/SolidColor.h"
#include "Material/ReflectingDielectric.h"
#include "Material/Utilities.h"
#include "Core/utils.h"

class HitRecord;
class Ray;
class RenderContext;
class Color;

class MetalLighting {
public:
  MetalLighting(int phong_exponent) : _phong_exponent(phong_exponent) {}
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
    if(costheta > 0){
      normal = -normal;
    } else {
      costheta = -costheta;
    }
    
    // Calculate lighting.
    const Object* world = scene->getObject();
    Light*const* begin = &lights[0];
    Light*const* end = &lights[lights.size()];
    
    ReflectingShader s(-ray.direction(),
		       normal,
		       _phong_exponent );
    result += MaterialUtilities::GetLightColor( begin, end, context,
						hitpos, normal, world, s )
      * context.originalColor;
  }
  bool bounce( Ray& newDirection, bool& diffuse, double random, const HitRecord &hit, const Ray &incoming) const
  {
    NOT_DONE("MetalLighting::bounce");
    return false;
  }

private:
  int _phong_exponent;
};

typedef _Material<SolidColor,MetalLighting,ReflectingDielectric> _MetalMaterial;
#define MetalMaterial(a,b) \
  _MetalMaterial(SolidColor(a),MetalLighting(b),ReflectingDielectric())
#define GlossyMetalMaterial(a,b,c)					\
  _MetalMaterial(SolidColor(a),MetalLighting(b),ReflectingDielectric(1.0,c))

#endif
