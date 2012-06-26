#ifndef PhongLighting_h
#define PhongLighting_h

#include "Core/Color.h"
#include "Core/Vector.h"

class HitRecord;
class Ray;
class RenderContext;

class PhongLighting {
public:
  PhongLighting(float Kd, float Ka, const Color& phong_color, int phong_exponent) :
    _Kd(Kd),_Ka(Ka),_phong_color(phong_color),_phong_exponent(phong_exponent) {}
  
  void operator()(Color& result, RenderContext& context, const Ray& ray,
		  const HitRecord& hit, const Color& atten, int depth) const;
  bool bounce( Ray& newDirection, bool& diffuse, double random, const HitRecord &hit, const Ray &incoming) const;
private:
  float _Kd;
  float _Ka;
  Color _phong_color;
  int   _phong_exponent;
  
};


struct PhongShader {
  friend std::ostream& operator<<(std::ostream &o, const PhongShader &s );
public:
  PhongShader( double Kd, const Vector &ray,
	       const Color &phong_color, const Color &color,
	       const Vector &normal,
	       int phong_exponent ) : _Kd(Kd),_ray(ray),
				      _phong_color(phong_color), _color(color),
				      _normal(normal),
				      _phong_exponent(phong_exponent)
  {}
  PhongShader( double Kd, const Color &phong_color, int phong_exponent ) :
    _Kd(Kd), _phong_color(phong_color), _phong_exponent(phong_exponent) {}
  
  void ray( const Vector &ray ) { _ray = ray; }
  void color( const Color &value ) { _color = value; }
  void normal( const Vector &normal ) { _normal = normal; }
  
  inline Color operator()( double cosphi,
		    const Color& light_color,
		    const Vector &light_direction ) {
    
    Color difflight(light_color*(_Kd*cosphi) * _color);
    
    Vector H = light_direction - _ray;
    H.normalize();
    double cos_alpha = Dot(H, _normal);
    if(cos_alpha > 0){
      double phong_term = ipow(cos_alpha, _phong_exponent);
      difflight += light_color*phong_term;
    }

    return difflight;
  }

  
private:
  double _Kd;
  Vector _ray;
  Color  _phong_color;
  Color  _color;
  Vector _normal;
  int    _phong_exponent;
};

#endif
