
#include "Core/Point.h"
#include <iostream>

std::ostream& operator<<(std::ostream& out, const Point& p)
{
  out << '[' << p.x() << ", " << p.y() << ", " << p.z() << ']';
  return out;
}

int
Point::overlap( double a, double b, double e ) const
{
  double hi, lo, h, l;
  
  hi = a + e;
  lo = a - e;
  h  = b + e;
  l  = b - e;

  if ( ( hi > l ) && ( lo < h ) )
    return 1;
  else
    return 0;
}
  
int
Point::inInterval( const Point &a, double epsilon ) const
{
  return ( overlap( x(), a.x(), epsilon ) &&
	   overlap( y(), a.y(), epsilon ) );
}


std::ostream& operator<<(std::ostream& out, const Point2D& p)
{
  out << '[' << p.x() << ", " << p.y() << ']';
  return out;
}

int
Point2D::overlap( double a, double b, double e ) const
{
  double hi, lo, h, l;
  
  hi = a + e;
  lo = a - e;
  h  = b + e;
  l  = b - e;

  if ( ( hi > l ) && ( lo < h ) )
    return 1;
  else
    return 0;
}
  
int
Point2D::inInterval( const Point2D &a, double epsilon ) const
{
  return ( overlap( x(), a.x(), epsilon ) &&
	   overlap( y(), a.y(), epsilon ) );
}
