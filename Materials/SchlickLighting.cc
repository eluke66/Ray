#include "Material/SchlickLighting.h"
#include "Core/Vector.h"
#include "Core/Color.h"
#include "Core/Scene.h"
#include "Core/Ray.h"
#include "Core/HitRecord.h"
#include "Core/RenderContext.h"
#include "Geometry/Primitive.h"
#include "Material/Utilities.h"
#include "Core/utils.h"

struct SchlickShader {
public:
  SchlickShader( const SchlickLighting &l,
		 const Vector &r,
		 const Vector &n) :
    _l(l), _ray(r), _normal(n) {}
  Color operator()( double cosphi,
		    const Color& light_color,
		    const Vector &light_direction );

private:
  const SchlickLighting &_l;
  Vector _ray;
  Vector _normal;
  
};




SchlickLighting::SchlickLighting(double F0, double sigma, double psi ) :
  _F0(F0), _sigma(sigma), _psi(psi)
{
  assert( sigma >= 0.0 && sigma <= 1.0 );
  assert( psi >= 0.0 && psi <= 1.0 );

  // Set glossiness, diffuse, specular factors.
  _g = 4 * _sigma * (1 - _sigma );
  if ( _sigma < 0.5 ) {
    _d = 0;
    _s = 1 - _g;
  }
  else {
    _d = 1 - _g;
    _s = 0;
  }
}

void
SchlickLighting::operator()(Color& result, RenderContext& context,
			    const Ray& ray, const HitRecord& hit,
			    const Color& atten, int depth) const
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

  Color light;

  Light*const* begin = &lights[0];
  Light*const* end = &lights[lights.size()];

  light += MaterialUtilities::GetLightColor( begin, end, context,
					     hitpos, normal, world,
					     SchlickShader(*this,
							   ray.direction(),
							   normal) );
  result = light*result;
}

bool
SchlickLighting::bounce( Ray& newDirection, bool& diffuse,
			 double random, const HitRecord &hit,
			 const Ray &incoming) const {
  NOT_DONE("SchlickLighting::bounce");
  return false;
}

#ifdef OLD_WAY
Color
SchlickShader::operator()( double cosphi,
			   const Color& light_color,
			   const Vector &light_direction )
{
  // Calculate parameters
  // w = -_ray
  // w' = light_direction
  Vector H = light_direction - _ray;
  H.normalize();

  double u = Dot(-_ray, H);
  double t = Dot(_normal, H);
  double v = Dot(-_ray, _normal );
  double vp = Dot(light_direction, _normal );


  Vector wf = H - _normal * t;
  wf.normalize();

  Vector T(1,0,0);
  double w = Dot(T, wf );
  
  // Specular factor
  double Su = _l._F0 + ( 1 - _l._F0 ) * ipow((1 - u),5);

  // Z, A, and G
  double Zt1 = ( 1 + _l._sigma*t*t - t*t);
  double Zt = _l._sigma / (Zt1 * Zt1 );

#ifdef USE_PSI
  double psi = _l._psi;
  double Aw = sqrt( psi / ( psi*psi - psi*psi*w*w + w*w ) );

  double Gv = v / ( v * (1 - _l._sigma) + _l._sigma );
  double Gvp = vp / ( vp * (1 - _l._sigma) + _l._sigma );
    
  double D =
    (Gv * Gvp * Zt * Aw / ( w*M_PI*v*vp ) ) +
    (Aw / M_PI) * ( 1 - Gv * Gvp );
#else
  double Gv = v / ( v * (1 - _l._sigma) + _l._sigma );
  double Gvp = vp / ( vp * (1 - _l._sigma) + _l._sigma );
    
  double D =
    (Gv * Gvp * Zt / ( w*M_PI*v*vp ) ) +
    ( 1 - Gv * Gvp ) / M_PI;
  
#endif

  double frs = 3;
  double Brdf = Su *  (_l._d / M_PI + _l._g * D + _l._s * frs );

  return light_color * Brdf;
}

#else

Color
SchlickShader::operator()( double cosphi,
			   const Color& light_color,
			   const Vector &light_direction )
{
  double r = _l._sigma;
  double p = _l._psi;
  
  // Precompute some parameters
  Vector H = light_direction - _ray;
  H.normalize();

  double u = Dot(-_ray, H);
  double t = Dot(_normal, H);
  double v = Dot(-_ray, _normal );
  double vp = Dot(light_direction, _normal );
  
  Vector wf = H - _normal * t;
  wf.normalize();
  Vector surfaceTangent( _normal.orthonormal() );
  surfaceTangent.normalize();
  double w = Dot(surfaceTangent, wf );
  
  // BRDF = Spectral * Diffuse

  //////////////////////////////////////////////////////
  // Calculate Specular component S
  double S = _l._F0 + ( 1 - _l._F0 ) * ipow((1 - u),5);
  //////////////////////////////////////////////////////

  
  //////////////////////////////////////////////////////
  // Calculate Diffuse component

  double factor = 1.0 / (4.0*M_PI*v*vp);
  double rt = 1 + r*t*t - t*t;
  double Z = r / (rt*rt); // Zenith factor

  double A = sqrt(p / (p*p - p*p*w*w+w*w)); // Azimuth factor

  double D = _l._d / M_PI + _l._g * rt*Z*A + _l._s / vp;
  //////////////////////////////////////////////////////

  double BRDF = S * D;

  return light_color * BRDF;
}

#endif
