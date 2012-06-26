#include "Material/PhongLighting.h"
#include "Geometry/Primitive.h"
#include "Core/Scene.h"
#include "Core/Ray.h"
#include "Core/Light.h"
#include "Material/Utilities.h"
#include "Core/utils.h"


void PhongLighting::operator()(Color& result, RenderContext& context, const Ray& ray,
			       const HitRecord& hit, const Color& atten, int depth) const {
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

  const Object* world = scene->getObject();
  Light*const* begin = &lights[0];
  Light*const* end = &lights[lights.size()];

  Color light = scene->getAmbient()*_Ka*result; // ambient*color
  
  PhongShader s( _Kd, ray.direction(),
		 _phong_color, result,
		 normal,
		 _phong_exponent );
  light += MaterialUtilities::GetLightColor( begin, end, context,
					     hitpos, normal, world, s );

  
  result = light;

}

bool
PhongLighting::bounce( Ray& newDirection, bool& diffuse,
			 double random, const HitRecord &hit,
			 const Ray &incoming) const {
  PRINT_ONCE("PhongLighting::bounce is not done - using lambertian!!!");

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


std::ostream& operator<<(std::ostream &o, const PhongShader &s ) {
  o << "Kd = " << s._Kd << ", color is " << s._color << " ray is " << s._ray << ", normal is " << s._normal; 
  o << "Phong Color is " << s._phong_color << ", exp is " << s._phong_exponent;
  return o;
}
