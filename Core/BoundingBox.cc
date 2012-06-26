
#include "Core/BoundingBox.h"
#include <float.h>
#include <iostream>
#include <assert.h>

BoundingBox::BoundingBox()
{
  min = Point(DBL_MAX, DBL_MAX, DBL_MAX);
  max = Point(-DBL_MAX, -DBL_MAX, -DBL_MAX);
}

void BoundingBox::reset() {
  min = Point(DBL_MAX, DBL_MAX, DBL_MAX);
  max = Point(-DBL_MAX, -DBL_MAX, -DBL_MAX);
}

BoundingBox::~BoundingBox()
{
}

void BoundingBox::extend(const Point& p)
{
  min = Min(p, min);
  max = Max(p, max);
}

bool BoundingBox::inside( const Point &p ) const {
  return
    p.x() >= min.x() &&
    p.y() >= min.y() &&
    p.z() >= min.z() &&
    p.x() <= max.x() &&
    p.y() <= max.y() &&
    p.z() <= max.z();
}

bool BoundingBox::overlaps( const BoundingBox &b ) const {
  
  // Try on x/y plane
  // r1 = this, r2 = b
  // L/R = x, T/B = y
  bool xyIntersect = ! (b.getMin().x() > max.x() ||
			b.getMax().x() < min.x() ||
			b.getMin().y() > max.y() ||
			b.getMax().y() > min.y() );
  if ( !xyIntersect )
    return false;
			
  // Try on x/z plane
  return ! (b.getMin().x() > max.x() ||
	    b.getMax().x() < min.x() ||
	    b.getMin().z() > max.z() ||
	    b.getMax().z() > min.z() );
  

  
  
}

std::ostream& operator<<(std::ostream& o, const BoundingBox& v1) {
  o << v1.getMin() << " -> " << v1.getMax();
  return o;
}

//////////////////// 2D ////////////////////////////////

BoundingBox2D::BoundingBox2D()
{
  min = Point2D(DBL_MAX, DBL_MAX);
  max = Point2D(-DBL_MAX, -DBL_MAX);
}

void BoundingBox2D::reset() {
  min = Point2D(DBL_MAX, DBL_MAX);
  max = Point2D(-DBL_MAX, -DBL_MAX);
}

BoundingBox2D::~BoundingBox2D()
{
}

void BoundingBox2D::extend(const Point2D& p)
{
  min = Min(p, min);
  max = Max(p, max);
}

bool BoundingBox2D::inside( const Point2D &p ) const {
  bool result = 
    p.x() >= min.x() &&
    p.y() >= min.y() &&
    p.x() <= max.x() &&
    p.y() <= max.y();

  return result;
}

#define S1(a) (a.x() <= min.x() && a.y() <= min.y() )
#define S2(a) (a.x() > min.x()  && a.x() <= max.x() && a.y() <= min.y() )
#define S3(a) (a.x() > max.x()  && a.y() <= min.y() )

#define S4(a) (a.x() <= min.x() && a.y() <= max.y() && a.y() > min.y() )
#define S5(a) (a.x() > min.x()  && a.x() <= max.x() && a.y() <= max.y() )
#define S6(a) (a.x() > max.x()  && a.y() <= max.y() && a.y() > min.y() )

#define S7(a) (a.x() <= min.x() && a.y() > max.y() )
#define S8(a) (a.x() > min.x()  && a.x() <= max.x() && a.y() > min.y() )
#define S9(a) (a.x() > max.x()  && a.y() > max.y() )

bool BoundingBox2D::overlaps( const BoundingBox2D &b ) const {
  /*
    return ! (b.getMin().x() >= max.x() ||
    b.getMax().x() <= min.x() ||
    b.getMin().y() >= max.y() ||
    b.getMax().y() >= min.y() );
  */

  Point2D min(getMin()), max(getMax());

  if ( S1(b.getMin()) )
    return S5(b.getMax()) || S6(b.getMax()) || S7(b.getMax()) || S8(b.getMax()) || S9(b.getMax());
  else if ( S2(b.getMin()) )
    return !S3(b.getMax());
  else if ( S3(b.getMin()) )
    return false;
  else if ( S4(b.getMin()) )
    return !S7(b.getMax());
  else if ( S5(b.getMin()) )
    return true;
  else if ( S6(b.getMin()) )
    return false;
  else if ( S7(b.getMin()) )
    return false;
  else if ( S8(b.getMin()) )
    return false;
  else if ( S9(b.getMin()) )
    return false;

  assert(0);
  return false;
}

std::ostream& operator<<(std::ostream& o, const BoundingBox2D& v1) {
  o << v1.getMin() << " -> " << v1.getMax();
  return o;
}
