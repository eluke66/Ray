#ifndef ReflectingDielectric_h
#define ReflectingDielectric_h

#include "Core/Vector.h"
#include "Core/Color.h"

class RenderContext;
class Ray;
class HitRecord;
class Blur;

struct ReflectingShader {
public:
  ReflectingShader( const Vector &direction,
		    const Vector &normal, int phong_exponent ) :
    _direction(direction), _normal(normal), _phong_exponent(phong_exponent)
  {
  }
  inline Color operator()( double cosphi,
			   const Color& light_color,
			   const Vector &light_direction ) {
    Color result;
    Vector H = light_direction + _direction;
    H.normalize();
    double cos_alpha = Dot(H, _normal);
    if(cos_alpha > 0){
      double phong_term = ipow(cos_alpha, _phong_exponent);
      result = light_color*phong_term;
    }
    return result;
  }

private:
  Vector _direction;
  Vector _normal;
  int    _phong_exponent;
};

class ReflectingDielectric {
public:
  ReflectingDielectric( double reflectivity=1.0, Blur *blur=NULL ) :
    _reflectivity(reflectivity), _blur(blur) {}
  
  void  getBlur( RenderContext &context,
		 const Ray &ray,
		 const HitRecord &hit, double &blur, int &numSamples ) const;
  bool bounce( Ray& newDirection, bool& diffuse, double random, const HitRecord &hit, const Ray &incoming) const;
  bool generatesCaustics() const { return true; }
  Color computeColor( const Vector &norm, RenderContext &context,
		      const Ray& ray,
		      const HitRecord& hit,
		      const Color& atten, int depth) const;
  
  void operator()(Color& result, RenderContext &context, const Ray& ray,
		  const HitRecord& hit, const Color& atten, int depth) const;
private:
  double _reflectivity;
  Blur  *_blur;
};

#endif
