
#ifndef Vector_h
#define Vector_h


#include "Core/Math.h"
#include <iosfwd>
#include <math.h>
#include <stdlib.h>

class Point;
class Vector {
 public:
  Vector() {
  }
  Vector(double x, double y, double z) {
    data[0] = x; data[1] = y; data[2] = z;
  }
  Vector(double v) {
    data[0] = data[1] = data[2] = v;
  }
  inline explicit Vector(const Point& copy);

  Vector(const Vector& copy) {
    for(int i=0;i<3;i++)
      data[i] = copy.data[i];
  }

  Vector& operator=(const Vector& copy) {
    for(int i=0;i<3;i++)
      data[i] = copy.data[i];
    return *this;
  }

  ~Vector() {
  }

  double x() const {
    return data[0];
  }
  void x(double value) {
    data[0] = value;
  }
  double y() const {
    return data[1];
  }
  void y(double value) {
    data[1] = value;
  }
  double z() const {
    return data[2];
  }
  void z(double value) {
    data[2] = value;
  }
  void set( double x, double y, double z ) {
    data[0] = x; data[1] = y; data[2] = z;
  }
  void setIdx(short int index, double value ) {
    data[index] = value;
  }
  Vector operator+(const Vector& v) const {
    return Vector(data[0]+v.data[0], data[1]+v.data[1], data[2]+v.data[2]);
  }
  Vector operator-(const Vector& v) const {
    return Vector(data[0]-v.data[0], data[1]-v.data[1], data[2]-v.data[2]);
  }
  Vector operator-() const {
    return Vector(-data[0], -data[1], -data[2]);
  }
  Vector& operator+=(const Vector& v) {
    data[0]+=v.data[0]; data[1]+=v.data[1]; data[2]+=v.data[2];
    return *this;
  }
  Vector& operator-=(const Vector& v) {
    data[0]-=v.data[0]; data[1]-=v.data[1]; data[2]-=v.data[2];
    return *this;
  }

  Vector operator*(const Vector& v) const {
    return Vector(data[0]*v.data[0], data[1]*v.data[1], data[2]*v.data[2]);
  }
  Vector operator*(double s) const {
    return Vector(data[0]*s, data[1]*s, data[2]*s);
  }
  Vector& operator*=(const Vector& v) {
    data[0]*=v.data[0]; data[1]*=v.data[1]; data[2]*=v.data[2];
    return *this;
  }
  Vector& operator*=(double s) {
    data[0]*=s; data[1]*=s; data[2]*=s;
    return *this;
  }
  Vector operator/(double s) const {
    double inv_s = 1./s;
    return Vector(data[0]*inv_s, data[1]*inv_s, data[2]*inv_s);
  }
  Vector& operator/=(double s) {
    double inv_s = 1./s;
    data[0]*=inv_s; data[1]*=inv_s; data[2]*=inv_s;
    return *this;
  }
  Vector operator/(const Vector& v) const {
    return Vector(data[0]/v.data[0], data[1]/v.data[1], data[2]/v.data[2]);
  }

  double length() const {
    return sqrt(data[0]*data[0]+data[1]*data[1]+data[2]*data[2]);
  }
  double length2() const {
    return data[0]*data[0]+data[1]*data[1]+data[2]*data[2];
  }

  static Vector Rand() {
    return Vector( 2.0 * drand48() - 1.0,
		   2.0 * drand48() - 1.0,
		   2.0 * drand48() - 1.0 );
  }

  void perturb( double val ) {
    *this += Rand() * val;
    normalize();
  }
  
  double normalize() {
    double l = length();
    double scale = 1./l;
    *this *= scale;
    return l;
  }
  double minComponent() const {
    return Min(data[0], data[1], data[2]);
  }
  double maxComponent() const {
    return Max(data[0], data[1], data[2]);
  }

  Vector inverse() const {
    return Vector(1./data[0], 1./data[1], 1./data[2]);
  }
  Vector absoluteValue() const {
    return Vector(Abs(data[0]), Abs(data[1]), Abs(data[2]));
  }

  static Vector zero() {
    return Vector(0,0,0);
  }
  static Vector one() {
    return Vector(1,1,1);
  }
  static Vector CosineWeightedVector( const Vector &normal );
  Vector orthonormal() const;
 private:
  double data[3];
};




////////////////////////////////// VECTOR 2D
////////////////////////////////// ///////////////////////////////////
class Point2D;
class Vector2D {
 public:
  Vector2D() {
  }
  Vector2D(double x, double y) {
    data[0] = x; data[1] = y;
  }
  Vector2D(double v) {
    data[0] = data[1] = v;
  }
  inline explicit Vector2D(const Point2D& copy);

  Vector2D(const Vector2D& copy) {
    for(int i=0;i<2;i++)
      data[i] = copy.data[i];
  }

  Vector2D& operator=(const Vector2D& copy) {
    for(int i=0;i<2;i++)
      data[i] = copy.data[i];
    return *this;
  }

  ~Vector2D() {
  }

  double x() const {
    return data[0];
  }
  double y() const {
    return data[1];
  }

  Vector2D operator+(const Vector2D& v) const {
    return Vector2D(data[0]+v.data[0], data[1]+v.data[1]);
  }
  Vector2D operator-(const Vector2D& v) const {
    return Vector2D(data[0]-v.data[0], data[1]-v.data[1]);
  }
  Vector2D operator-() const {
    return Vector2D(-data[0], -data[1]);
  }
  Vector2D& operator+=(const Vector2D& v) {
    data[0]+=v.data[0]; data[1]+=v.data[1];
    return *this;
  }
  Vector2D& operator-=(const Vector2D& v) {
    data[0]-=v.data[0]; data[1]-=v.data[1];
    return *this;
  }

  Vector2D operator*(const Vector2D& v) const {
    return Vector2D(data[0]*v.data[0], data[1]*v.data[1]);
  }
  Vector2D operator*(double s) const {
    return Vector2D(data[0]*s, data[1]*s);
  }
  Vector2D& operator*=(const Vector2D& v) {
    data[0]*=v.data[0]; data[1]*=v.data[1]; 
    return *this;
  }
  Vector2D& operator*=(double s) {
    data[0]*=s; data[1]*=s; 
    return *this;
  }
  Vector2D operator/(double s) const {
    double inv_s = 1./s;
    return Vector2D(data[0]*inv_s, data[1]*inv_s);
  }
  Vector2D& operator/=(double s) {
    double inv_s = 1./s;
    data[0]*=inv_s; data[1]*=inv_s;
    return *this;
  }
  Vector2D operator/(const Vector2D& v) const {
    return Vector2D(data[0]/v.data[0], data[1]/v.data[1]);
  }

  double length() const {
    return sqrt(data[0]*data[0]+data[1]*data[1]);
  }
  double length2() const {
    return data[0]*data[0]+data[1]*data[1];
  }

  static Vector2D Rand() {
    return Vector2D( 2.0 * drand48() - 1.0,
		     2.0 * drand48() - 1.0);
  }

  void perturb( double val ) {
    *this += Rand() * val;
    normalize();
  }
  
  double normalize() {
    double l = length();
    double scale = 1./l;
    *this *= scale;
    return l;
  }
  double minComponent() const {
    return Min(data[0], data[1]);
  }
  double maxComponent() const {
    return Max(data[0], data[1]);
  }

  Vector2D inverse() const {
    return Vector2D(1./data[0], 1./data[1]);
  }
  Vector2D absoluteValue() const {
    return Vector2D(Abs(data[0]), Abs(data[1]));
  }

  static Vector2D zero() {
    return Vector2D(0,0);
  }
  static Vector2D one() {
    return Vector2D(1,1);
  }

 private:
  double data[2];
};

#include "Point.h"

inline Vector::Vector(const Point& copy)
{
  data[0] = copy.x(); data[1] = copy.y(); data[2] = copy.z();
}

inline Vector operator*(double s, const Vector& v) {
  return Vector(s*v.x(), s*v.y(), s*v.z());
}

inline double Dot(const Vector& v1, const Vector& v2)
{
  return v1.x()*v2.x() + v1.y()*v2.y() + v1.z()*v2.z();
}

inline Vector Cross(const Vector& v1, const Vector& v2)
{
  return Vector(v1.y()*v2.z() - v1.z()*v2.y(),
                v1.z()*v2.x() - v1.x()*v2.z(),
                v1.x()*v2.y() - v1.y()*v2.x());
}

inline Vector Min(const Vector& p1, const Vector& p2)
{
  return Vector(Min(p1.x(), p2.x()), Min(p1.y(), p2.y()), Min(p1.z(), p2.z()));
}

inline Vector Max(const Vector& p1, const Vector& p2)
{
  return Vector(Max(p1.x(), p2.x()), Max(p1.y(), p2.y()), Max(p1.z(), p2.z()));
}

std::ostream& operator<<(std::ostream&, const Vector& v);

inline Vector2D::Vector2D(const Point2D& copy)
{
  data[0] = copy.x(); data[1] = copy.y();
}

inline Vector2D operator*(double s, const Vector2D& v) {
  return Vector2D(s*v.x(), s*v.y());
}

inline double Dot(const Vector2D& v1, const Vector2D& v2)
{
  return v1.x()*v2.x() + v1.y()*v2.y() ;
}

inline Vector2D Min(const Vector2D& p1, const Vector2D& p2)
{
  return Vector2D(Min(p1.x(), p2.x()), Min(p1.y(), p2.y()));
}

inline Vector2D Max(const Vector2D& p1, const Vector2D& p2)
{
  return Vector2D(Max(p1.x(), p2.x()), Max(p1.y(), p2.y()));
}

std::ostream& operator<<(std::ostream&, const Vector2D& v);

#endif
