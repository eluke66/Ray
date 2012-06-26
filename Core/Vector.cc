
#include "Core/Vector.h"
#include "Core/Matrix.h"
#include "Core/utils.h"
#include <iostream>

std::ostream& operator<<(std::ostream& out, const Vector& v)
{
  out << '[' << v.x() << ", " << v.y() << ", " << v.z() << ']';
  return out;
}

std::ostream& operator<<(std::ostream& out, const Vector2D& v)
{
  out << '[' << v.x() << ", " << v.y() << ']';
  return out;
}


Vector Vector::CosineWeightedVector( const Vector &normal )
{
  
  /*
    x = sin(T)cos(P)
    y = sin(T)sin(P)
    z = cos(T)

    But these are relative to a normal vector of <0,0,1>

    Get the new vector
    Get a rot matrix that aligns the normal with 0,0,1
    Invert the matrix, and multiply it with the new vector
    
  */

  double costheta = sqrt(drand48());
  double theta = acos(costheta);
  double phi = 2.0 * M_PI * drand48();
  Vector newVector( sin(theta) * cos(phi),
		    sin(theta) * sin(phi),
		    costheta );
  
  Matrix4 mat = Matrix4(Matrix3::AlignVectorWithAxis( normal, Matrix3::Z_AXIS )).invert();

  return mat * newVector;
  
}

Vector Vector::orthonormal() const {
  const double eps = 1e-8;

  // Case where X component of vector is 0
  if ( ABSCLOSE(x(),0,eps) ) {
    if ( ABSCLOSE(y(),0,eps) )
      return Vector(1,1,0);
    if ( ABSCLOSE(z(),0,eps) )
      return Vector(1,0,1);
    
    //return Vector(1,1,-x()/z());
    return Vector(1,0,0);
  }

  // Case where Y component of vector is 0
  if ( ABSCLOSE(y(),0,eps) ) {
    if ( ABSCLOSE(z(),0,eps) )
      return Vector(0,1,1);
    return Vector(1,0,-x()/z());
  }

  // Case where Z component of vector is 0
  if ( ABSCLOSE(z(),0,eps) )
    return Vector(1,-x()/y(), 0 );

  // Mixed vector
  return Vector( 1, 1, -( x() + y() ) / z() );
}
