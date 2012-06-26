
#ifndef BoundingBox_h
#define BoundingBox_h

#include "Core/Point.h"

class BoundingBox {
 public:
  BoundingBox();
  BoundingBox(const Point &pmin, const Point &pmax ) :
    min(pmin), max(pmax) {}
  ~BoundingBox();

  void reset();
  void extend(const Point&);

  Point getMin() const {
    return min;
  }
  Point getMax() const {
    return max;
  }
  Point getCenter() const {
    return (max + min) * 0.5;
  }
  double getSize() const {
    return (getMax() - getMin()).length();
  }
  bool inside( const Point &p ) const;
  bool overlaps( const BoundingBox &b ) const;
  int maxComponent() const {
    Vector v(max-min);
    if ( v.x() > v.y() && v.x() > v.z() )
      return 0;
    if ( v.y() > v.z() )
      return 1;
    return 2;
  }
 private:
  Point min, max;
};

std::ostream& operator<<(std::ostream&, const BoundingBox& v1);

////////////////////////////////// 2D ///////////////////////////
class BoundingBox2D {
 public:
  BoundingBox2D();
  BoundingBox2D(const Point2D &pmin, const Point2D &pmax ) :
    min(pmin), max(pmax) {}
  ~BoundingBox2D();

  void reset();
  void extend(const Point2D&);

  Point2D getMin() const {
    return min;
  }
  Point2D getMax() const {
    return max;
  }
  Point2D getCenter() const {
    return (max + min) * 0.5;
  }
  double getSize() const {
    return (getMax() - getMin()).length();
  }
  bool inside( const Point2D &p ) const;
  bool overlaps( const BoundingBox2D &b ) const;
 private:
  Point2D min, max;
};

std::ostream& operator<<(std::ostream&, const BoundingBox2D& v1);
#endif
