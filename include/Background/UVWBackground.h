
#ifndef UVWBackground_h
#define UVWBackground_h

#include "Background/Background.h"
#include "Core/Vector.h"

class UVWBackground : public Background{
 public:
  UVWBackground( const Vector &pole = Vector(0,0,1),
		 const Vector &prime = Vector(0,-1,0) ) :
    _X(prime), _Y(Cross(prime, pole)), _Z(pole)
  {
    _X.normalize();
    _Y.normalize();
    _Z.normalize();
  }

protected:
  void getUVW(Vector &uvw, const Vector &ray);
  
private:
  Vector _X, _Y, _Z;
  UVWBackground(const UVWBackground&);
  UVWBackground& operator=(const UVWBackground&);
};


inline void
UVWBackground::getUVW(Vector &uvw, const Vector &ray) {
  double z = Dot(ray, _Z);
  double y = Dot(ray, _Y);
  double x = Dot(ray, _X);
  double R = sqrt(x*x+y*y+z*z);
  double theta = atan2(y, x);
  theta *= 1./(2*M_PI);
  double u = theta < 0? theta + 1 : theta;
  double phi = acos(z/R);
  double v = 1-phi/M_PI;
  uvw = Vector(u, v, R);
}

#endif

