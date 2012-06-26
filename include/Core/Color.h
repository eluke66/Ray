
#ifndef Color_h
#define Color_h

#include "Core/Math.h"
#include <iosfwd>
#include <math.h>
#include <assert.h>

class Color {
 public:
  Color() {
    for(int i=0;i<3;i++)
      data[i] = 0.0f;
    
  }

  Color(const Color& copy) {
    for(int i=0;i<3;i++)
      data[i] = copy.data[i];
  }

  Color& operator=(const Color& copy) {
    for(int i=0;i<3;i++)
      data[i] = copy.data[i];
    return *this;
  }

  Color( float c[4] ) {
    for(int i=0;i<3;i++)
      data[i] = c[i];
  }

  Color( float c ) {
    for(int i=0;i<3;i++)
      data[i] = c;
  }
  
  ~Color() {
  }

  double r() const {
    return data[0];
  }
  double R() const {
    return data[0];
  }
  double g() const {
    return data[1];
  }
  double b() const {
    return data[2];
  }
  double G() const {
    return data[1];
  }
  double B() const {
    return data[2];
  }

  Color(float r, float g, float b) {
    data[0] = r;
    data[1] = g;
    data[2] = b;
  }
  Color log() const {
    return Color(::log(data[0]), ::log(data[1]), ::log(data[2]));
  }
  Color operator+(const Color& c) const {
    return Color(data[0] + c.data[0], data[1]+c.data[1], data[2]+c.data[2]);
  }
  Color& operator+=(const Color& c) {
    data[0] += c.data[0]; data[1] += c.data[1]; data[2] += c.data[2];
    return *this;
  }
  Color operator-(const Color& c) const {
    return Color(data[0] - c.data[0], data[1]-c.data[1], data[2]-c.data[2]);
  }
  Color& operator-=(const Color& c) {
    data[0] -= c.data[0]; data[1] -= c.data[1]; data[2] -= c.data[2];
    return *this;
  }
  Color operator*(const Color& c) const {
    return Color(data[0] * c.data[0], data[1]*c.data[1], data[2]*c.data[2]);
  }
  Color& operator*=(const Color& c) {
    data[0] *= c.data[0]; data[1] *= c.data[1]; data[2] *= c.data[2];
    return *this;
  }
  Color operator*(double s) const {
    return Color(data[0]*s, data[1]*s, data[2]*s);
  }
  Color& operator*=(double s) {
    data[0] *= s; data[1] *= s; data[2] *= s;
    return *this;
  }
  Color operator*(float s) const {
    return Color(data[0]*s, data[1]*s, data[2]*s);
  }
  Color& operator*=(float s) {
    data[0] *= s; data[1] *= s; data[2] *= s;
    return *this;
  }
  Color operator/(double s) const {
    return Color(data[0]/s, data[1]/s, data[2]/s);
  }
  Color& operator/=(double s) {
    data[0] /= s; data[1] /= s; data[2] /= s;
    return *this;
  }
  Color operator/(float s) const {
    return Color(data[0]/s, data[1]/s, data[2]/s);
  }
  Color& operator/=(float s) {
    data[0] /= s; data[1] /= s; data[2] /= s;
    return *this;
  }

  float power() const {
    return data[0] + data[1] + data[2];
  }
  
  float maxComponent() const {
    return Max(data[0], data[1], data[2]);
  }

  void normalize() {
    float mc = maxComponent();
    data[0] /= mc; data[1] /= mc; data[2] /= mc;
  };

  // Frac = fraction of c1. 0 = all c2, 1.0 = all c1
  static Color Lerp( const Color &c1, const Color &c2, float frac );
  
 private:
  float data[3];
};

std::ostream& operator<<(std::ostream&, const Color& c);

#endif
