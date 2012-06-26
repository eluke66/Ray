#include "Material/OrenNayarLighting.h"

#include "Geometry/Primitive.h"
#include "Core/Scene.h"
#include "Core/Ray.h"
#include "Core/Light.h"
#include "Material/Utilities.h"
#include "Core/utils.h"

struct OrenNayarShader {
public:
  OrenNayarShader( double A, double B, float Kd, double costheta ) : _A(A), _B(B), _costheta(costheta),
								     _Kd(Kd) {}
  inline Color operator()( double cosphi, const Color& light_color, const Vector &light_direction ) {
    double theta = acos(_costheta);
    double phi = acos(cosphi);
    double alpha = Max(theta, phi);
    double beta = Min(theta, phi);
    double fr = _A+_B*Max(0.0, cos(theta-phi))*sin(alpha)*tan(beta);
    return light_color * cosphi * fr * _Kd;
  }

private:
  double _A, _B, _costheta;
  float _Kd;
};


OrenNayarLighting::OrenNayarLighting(float sigma, float Kd, float Ka) : _Kd(Kd), _Ka(Ka)
{
  double s2 = sigma*sigma;
  _A = 1-s2/(2*(s2+0.33));
  _B = 0.45*s2/(s2+0.09);
}

void OrenNayarLighting::operator()(Color& result, RenderContext& context, const Ray& ray,
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
  else
    costheta = -costheta;
  
  const Object* world = scene->getObject();

  Color light = scene->getAmbient()*_Ka;

  Light*const* begin = &lights[0];
  Light*const* end = &lights[lights.size()];

  light += MaterialUtilities::GetLightColor( begin, end, context,
					     hitpos, normal, world,
					     OrenNayarShader(_A, _B, _Kd,
							     costheta) );
  result = light*result;
}

bool
OrenNayarLighting::bounce( Ray& newDirection, bool& diffuse,
			 double random, const HitRecord &hit,
			 const Ray &incoming) const {
  NOT_DONE("OrenNayarLighting::bounce");
  return false;
}
