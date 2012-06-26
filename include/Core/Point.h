

#include "Core/Vector.h"

#ifndef Point_h
#define Point_h

#include <assert.h>
#include <iosfwd>
class Point2D;

class Point {
 public:
  Point() {
  }

  Point(double x, double y, double z) {
    data[0] = x; data[1] = y; data[2] = z;
  }
  Point(float indata[3]) {
    data[0] = indata[0]; data[1] = indata[1]; data[2] = indata[2];
  }

  Point(const Point& copy) {
    for(int i=0;i<3;i++)
      data[i] = copy.data[i];
  }
  
  explicit Point(const Vector& copy) {
    data[0] = copy.x(); data[1] = copy.y(); data[2] = copy.z();
  }
  explicit Point(const Point2D &copy);
  explicit Point(const Point2D &copy, double z);
  
  Point& operator=(const Point& copy) {
    for(int i=0;i<3;i++)
      data[i] = copy.data[i];
    return *this;
  }

  double idx(short int index ) const {
    assert(index >= 0 && index < 3 );
    return data[index];
  }
  void setIdx(short int index, double value ) {
    assert(index >= 0 && index < 3 );
    data[index] = value;
  }
  
  ~Point() {
  }

  double x() const {
    return data[0];
  }
  double y() const {
    return data[1];
  }
  double z() const {
    return data[2];
  }

  Vector operator-(const Point& p) const {
    return Vector(data[0]-p.data[0], data[1]-p.data[1], data[2]-p.data[2]);
  }
  Point operator+(const Vector& v) const {
    return Point(data[0]+v.x(), data[1]+v.y(), data[2]+v.z());
  }
  Point operator+(const Point& p) const {
    return Point(data[0]+p.x(), data[1]+p.y(), data[2]+p.z());
  }
  Point operator-(const Vector& v) const {
    return Point(data[0]-v.x(), data[1]-v.y(), data[2]-v.z());
  }
  Point operator*(double d) const {
    return Point(data[0]*d, data[1]*d, data[2]*d );
  }
  
  
  Point& operator+=(const Vector& v) {
    data[0]+=v.x();
    data[1]+=v.y();
    data[2]+=v.z();
    return *this;
  }

  Point& operator+=(const Point& p) {
    data[0]+=p.x();
    data[1]+=p.y();
    data[2]+=p.z();
    return *this;
  }

  Point& operator-=(const Vector& v) {
    data[0]-=v.x();
    data[1]-=v.y();
    data[2]-=v.z();
    return *this;
  }

  int overlap( double a, double b, double e ) const;
  int inInterval( const Point &a, double epsilon ) const;
  bool operator==(const Point &p) const {
    return inInterval(p, 1e-6);
  }
  bool operator<(const Point &p ) const {
    return ((Vector)*this).length2() <
      ((Vector)p).length2();
  }
 private:
  double data[3];
};

inline Point operator+(const Vector& v, const Point& p)
{
  return Point(v.x() + p.x(), v.y() + p.y(), v.z() + p.z());
}

inline Point Interpolate(double s2, const Point& p1, const Point& p2)
{
  double s1 = 1-s2;
  return Point(s1*p1.x() + s2*p2.x(), s1*p1.y() + s2*p2.y(),
               s1*p1.z() + s2*p2.z());
}

inline Point AffineCombination(double s1, const Point& p1,
                               double s2, const Point& p2)
{
  return Point(s1*p1.x() + s2*p2.x(), s1*p1.y() + s2*p2.y(),
               s1*p1.z() + s2*p2.z());
}

inline Point AffineCombination(double s1, const Point& p1,
                               double s2, const Point& p2,
                               double s3, const Point& p3)
{
  return Point(s1*p1.x() + s2*p2.x() + s3*p3.x(),
               s1*p1.y() + s2*p2.y() + s3*p3.y(),
               s1*p1.z() + s2*p2.z() + s3*p3.z());
}

inline double Dot(const Point& v1, const Vector& v2)
{
  return v1.x()*v2.x() + v1.y()*v2.y() + v1.z()*v2.z();
}

inline double Dot(const Vector& v1, const Point& v2)
{
  return v1.x()*v2.x() + v1.y()*v2.y() + v1.z()*v2.z();
}

inline Point Min(const Point& p1, const Point& p2)
{
  return Point(Min(p1.x(), p2.x()), Min(p1.y(), p2.y()), Min(p1.z(), p2.z()));
}

inline Point Max(const Point& p1, const Point& p2)
{
  return Point(Max(p1.x(), p2.x()), Max(p1.y(), p2.y()), Max(p1.z(), p2.z()));
}

std::ostream& operator<<(std::ostream&, const Point& v1);


//////////////////////////////// POINT 2D //////////////////////////////
class Vector2D;

class Point2D {
 public:
  Point2D() {
  }

  Point2D(double x, double y) {
    data[0] = x; data[1] = y;
  }
  Point2D(float indata[2]) {
    data[0] = indata[0]; data[1] = indata[1]; 
  }

  Point2D(const Point2D& copy) {
    for(int i=0;i<2;i++)
      data[i] = copy.data[i];
  }
  explicit Point2D(const Point &copy ) {
    data[0] = copy.x(); data[1] = copy.y();
  }
  explicit Point2D(const Vector2D& copy) {
    data[0] = copy.x(); data[1] = copy.y();
  }

  Point2D& operator=(const Point2D& copy) {
    for(int i=0;i<2;i++)
      data[i] = copy.data[i];
    return *this;
  }

  ~Point2D() {
  }

  double idx(short int idx) const {
    assert(idx >= 0);
    assert(idx < 2);
    return data[idx];
  }
  void setIdx(short int index, double value ) {
    assert(index >= 0 && index < 2 );
    data[index] = value;
  }
  
  double x() const {
    return data[0];
  }
  double y() const {
    return data[1];
  }

  Vector2D operator-(const Point2D& p) const {
    return Vector2D(data[0]-p.data[0], data[1]-p.data[1]);
  }
  Point2D operator+(const Vector2D& v) const {
    return Point2D(data[0]+v.x(), data[1]+v.y());
  }
  Point2D operator+(const Point2D& p) const {
    return Point2D(data[0]+p.x(), data[1]+p.y());
  }
  Point2D operator-(const Vector2D& v) const {
    return Point2D(data[0]-v.x(), data[1]-v.y());
  }
  Point2D operator*(double d) const {
    return Point2D(data[0]*d, data[1]*d);
  }
  
  
  Point2D& operator+=(const Vector2D& v) {
    data[0]+=v.x();
    data[1]+=v.y();
    return *this;
  }
  Point2D& operator+=(const Point2D& v) {
    data[0]+=v.x();
    data[1]+=v.y();
    return *this;
  }

  Point2D& operator-=(const Vector2D& v) {
    data[0]-=v.x();
    data[1]-=v.y();
    return *this;
  }

  int overlap( double a, double b, double e ) const;
  int inInterval( const Point2D &a, double epsilon ) const;
  bool operator==(const Point2D &p) const {
    return inInterval(p, 1e-6);
  }
  bool operator<(const Point2D &p ) const {
    return ((Vector2D)*this).length2() <
      ((Vector2D)p).length2();
  }
 private:
  double data[2];
};

inline Point2D operator+(const Vector2D& v, const Point2D& p)
{
  return Point2D(v.x() + p.x(), v.y() + p.y());
}

inline Point2D Interpolate(double s2, const Point2D& p1, const Point2D& p2)
{
  double s1 = 1-s2;
  return Point2D(s1*p1.x() + s2*p2.x(), s1*p1.y() + s2*p2.y());
}

inline Point2D AffineCombination(double s1, const Point2D& p1,
                               double s2, const Point2D& p2)
{
  return Point2D(s1*p1.x() + s2*p2.x(), s1*p1.y() + s2*p2.y());
}

inline Point2D AffineCombination(double s1, const Point2D& p1,
				 double s2, const Point2D& p2,
				 double s3, const Point2D& p3)
{
  return Point2D(s1*p1.x() + s2*p2.x() + s3*p3.x(),
               s1*p1.y() + s2*p2.y() + s3*p3.y());
}

inline double Dot(const Point2D& v1, const Vector2D& v2)
{
  return v1.x()*v2.x() + v1.y()*v2.y();
}

inline double Dot(const Vector2D& v1, const Point2D& v2)
{
  return v1.x()*v2.x() + v1.y()*v2.y();
}

inline Point2D Min(const Point2D& p1, const Point2D& p2)
{
  return Point2D(Min(p1.x(), p2.x()), Min(p1.y(), p2.y()));
}

inline Point2D Max(const Point2D& p1, const Point2D& p2)
{
  return Point2D(Max(p1.x(), p2.x()), Max(p1.y(), p2.y()));
}

std::ostream& operator<<(std::ostream&, const Point2D& v1);

inline Point::Point(const Point2D &copy) {
  data[0] = copy.x(); data[1] = copy.y(); data[2] = 0;
}

inline Point::Point(const Point2D &copy, double z) {
  data[0] = copy.x(); data[1] = copy.y(); data[2] = z;
}


#endif

