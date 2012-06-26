
#ifndef Ray_h
#define Ray_h

#include "Core/Point.h"
#include "Core/Vector.h"

class Ray {
 public:
  Ray() {
  }
  Ray(const Point& origin, const Vector& direction)
    : org(origin), dir(direction) {
    idir = dir.inverse();
  }
    
  ~Ray() {
  }
  Ray(const Ray& copy)
    : org(copy.org), dir(copy.dir),idir(copy.idir) {
  }
  Ray& operator=(const Ray& copy) {
    org = copy.org;
    dir = copy.dir;
    idir = copy.idir;
    return *this;
  }

  void set( const Point &origin, const Vector &direction ) {
    org = origin;
    dir = direction;
    idir = dir.inverse();
  }
  const Point& origin() const {
    return org;
  }
  const Vector& direction() const {
    return dir;
  }
  const Vector &inverseDirection() const {
    return idir;
  }
 private:
   Point org;
   Vector dir;
  Vector idir;
};

std::ostream& operator<<(std::ostream& out, const Ray& r);

#endif
