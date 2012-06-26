#include <stdio.h>
#include "Core/Matrix.h"
#include "Core/Vector.h"
#include "Core/Point.h"
#include "Core/utils.h"

Matrix4::Matrix4() {
  for (int i = 0; i < 4; i++)
    for (int j = 0; j < 4; j++)
      _data[i][j] = (double)0.0;
}

Matrix4::Matrix4(const Matrix4& m)
{
  for (int i = 0; i < 4; i++)
    for (int j = 0; j < 4; j++)
      _data[i][j] = m._data[i][j];
}

Matrix4::Matrix4(const Matrix3& m)
{
  identity();
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      _data[i][j] = m._data[i][j];
}

  
Matrix4::Matrix4(double newdata[4][4])
{
  for (int i = 0; i < 4; i++)
    for (int j = 0; j < 4; j++)
      _data[i][j] = newdata[i][j];
}

Matrix4::~Matrix4()
{
}

std::ostream &
operator<<(std::ostream &output, const Matrix4& m)
{
  output << std::endl;
  for (int i = 0 ; i < 4; i++)
    {
      for (int j = 0; j < 4; j++)
	output << m._data[i][j] << " ";
      output << std::endl;
    }
  return output;
}


Matrix4
Matrix4::operator *(const Matrix4& m) const
{
  double newdata[4][4];

  for (int i = 0; i < 4; i++)
    for (int j = 0; j < 4; j++)
      {
	newdata[i][j] = (double)0.0;
	for (int k = 0; k < 4; k++)
	  newdata[i][j] += _data[i][k] * m._data[k][j];
      }

  return Matrix4(newdata);
}


Vector
Matrix4::operator *(const Vector& v) const
{
  double newdata[4];

  for (int i = 0; i < 4; i++)
    {
      newdata[i] = _data[i][0] * v.x() +
	_data[i][1] * v.y() +
	_data[i][2] * v.z() +
	_data[i][3] * 1; // = v.w(), since vectors are homogeneous
    }
  // We return a homogeneous vector.
  return Vector(newdata[0]/newdata[3],
		newdata[1]/newdata[3],
		newdata[2]/newdata[3]);
  
}

Vector 
Matrix4::transform(const Vector &v ) const {
  double newdata[3];
  
  for (int i = 0; i < 4; i++)
    {
      newdata[i] = _data[i][0] * v.x() +
	_data[i][1] * v.y() +
	_data[i][2] * v.z();
    }
  return Vector( newdata[0], newdata[1], newdata[2] );
}

Point
Matrix4::operator *(const Point& p) const
{
  double newdata[4];

  for (int i = 0; i < 4; i++)
    {
      newdata[i] = _data[i][0] * p.x() +
	_data[i][1] * p.y() +
	_data[i][2] * p.z() +
	_data[i][3] * 1; // = p.w(), since points are homogeneous
    }
  // We return a homogeneous point.
  return Point(newdata[0]/newdata[3],
	       newdata[1]/newdata[3],
	       newdata[2]/newdata[3]);
  
}

bool
Matrix4::isIdentity() const {
  bool returnValue = true;
  
  for (int i = 0; i < 4; i++)
    for (int j = 0; j < 4; j++)
      if (i==j)
	returnValue = (returnValue && ABSCLOSE(_data[i][j], 1.0, 1e-6));
      else
	returnValue = (returnValue && ABSCLOSE(_data[i][j], 0.0, 1e-6));
  
  return returnValue;
}

void
Matrix4::identity()
{
  for (int i = 0; i < 4; i++)
    for (int j = 0; j < 4; j++)
      if (i==j)
	_data[i][j] = 1.0;
      else
	_data[i][j] = 0.0;
}

void
Matrix4::scale(double scale)
{
  _data[0][0] *= scale;
  _data[1][1] *= scale;
  _data[2][2] *= scale;
}

void
Matrix4::scale( const Vector &v ) {
  _data[0][0] *= v.x();
  _data[1][1] *= v.y();
  _data[2][2] *= v.z();
  
}

void
Matrix4::setRow(const Vector& v, int row)
{
  _data[row][0] = v.x();
  _data[row][1] = v.y();
  _data[row][2] = v.z();
}

void
Matrix4::setCol(const Vector& v, int col)
{
  _data[0][col] = v.x();
  _data[1][col] = v.y();
  _data[2][col] = v.z();
}

#define M11 _data[0][0]
#define M12 _data[0][1]
#define M13 _data[0][2]
#define M21 _data[1][0]
#define M22 _data[1][1]
#define M23 _data[1][2]
#define M31 _data[2][0]
#define M32 _data[2][1]
#define M33 _data[2][2]

#define M14 _data[0][3]
#define M24 _data[1][3]
#define M34 _data[2][3]
#define M44 _data[3][3]
#define M41 _data[3][0]
#define M42 _data[3][1]
#define M43 _data[3][2]


double
Matrix4::determinant() const {

  return (M11 * M22 - M12 * M21) * (M33 * M44 - M34 * M43)
    -(M11 * M23 - M13 * M21) * (M32 * M44 - M34 * M42)
    +(M11 * M24 - M14 * M21) * (M32 * M43 - M33 * M42)
    +(M12 * M23 - M13 * M22) * (M31 * M44 - M34 * M41)
    -(M12 * M24 - M14 * M22) * (M31 * M43 - M33 * M41)
    +(M13 * M24 - M14 * M23) * (M31 * M42 - M32 * M41);
}


Matrix4 
Matrix4::invert() const {

  double s = determinant();
  assert( s != 0.0 );
  s = 1.0/s;

  double data[4][4];
  data[0][0] =  s*(M22*(M33*M44 - M34*M43) + M23*(M34*M42 - M32*M44) + M24*(M32*M43 - M33*M42));
  data[0][1] =  s*(M32*(M13*M44 - M14*M43) + M33*(M14*M42 - M12*M44) + M34*(M12*M43 - M13*M42));
  data[0][2] =  		 s*(M42*(M13*M24 - M14*M23) + M43*(M14*M22 - M12*M24) + M44*(M12*M23 - M13*M22));
  data[0][3] =  		 s*(M12*(M24*M33 - M23*M34) + M13*(M22*M34 - M24*M32) + M14*(M23*M32 - M22*M33));
  data[1][0] =  		 s*(M23*(M31*M44 - M34*M41) + M24*(M33*M41 - M31*M43) + M21*(M34*M43 - M33*M44));
  data[1][1] =  		 s*(M33*(M11*M44 - M14*M41) + M34*(M13*M41 - M11*M43) + M31*(M14*M43 - M13*M44));
  data[1][2] =  		 s*(M43*(M11*M24 - M14*M21) + M44*(M13*M21 - M11*M23) + M41*(M14*M23 - M13*M24));
  data[1][3] =  		 s*(M13*(M24*M31 - M21*M34) + M14*(M21*M33 - M23*M31) + M11*(M23*M34 - M24*M33));
  data[2][0] =  		 s*(M24*(M31*M42 - M32*M41) + M21*(M32*M44 - M34*M42) + M22*(M34*M41 - M31*M44));
  data[2][1] =  		 s*(M34*(M11*M42 - M12*M41) + M31*(M12*M44 - M14*M42) + M32*(M14*M41 - M11*M44));
  data[2][2] =  		 s*(M44*(M11*M22 - M12*M21) + M41*(M12*M24 - M14*M22) + M42*(M14*M21 - M11*M24));
  data[2][3] =  		 s*(M14*(M22*M31 - M21*M32) + M11*(M24*M32 - M22*M34) + M12*(M21*M34 - M24*M31));
  data[3][0] =  		 s*(M21*(M33*M42 - M32*M43) + M22*(M31*M43 - M33*M41) + M23*(M32*M41 - M31*M42));
  data[3][1] =  		 s*(M31*(M13*M42 - M12*M43) + M32*(M11*M43 - M13*M41) + M33*(M12*M41 - M11*M42));
  data[3][2] =  		 s*(M41*(M13*M22 - M12*M23) + M42*(M11*M23 - M13*M21) + M43*(M12*M21 - M11*M22));
  data[3][3] =  		 s*(M11*(M22*M33 - M23*M32) + M12*(M23*M31 - M21*M33) + M13*(M21*M32 - M22*M31));

  return Matrix4(data);
}


Matrix3::Matrix3() {
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      _data[i][j] = (double)0.0;
}

Matrix3::Matrix3(const Matrix3& m)
{
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      _data[i][j] = m._data[i][j];
}

Matrix3::Matrix3(const Matrix4& m)
{
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      _data[i][j] = m._data[i][j];
}

  
Matrix3::Matrix3(double newdata[3][3])
{
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      _data[i][j] = newdata[i][j];
}

Matrix3::~Matrix3()
{
}

std::ostream &
operator<<(std::ostream &output, const Matrix3& m)
{
  output << std::endl;
  for (int i = 0 ; i < 3; i++)
    {
      for (int j = 0; j < 3; j++)
	output << m._data[i][j] << " ";
      output << std::endl;
    }
  return output;
}


Matrix3
Matrix3::operator *(const Matrix3& m) const
{
  double newdata[3][3];

  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      {
	newdata[i][j] = (double)0.0;
	for (int k = 0; k < 3; k++)
	  newdata[i][j] += _data[i][k] * m._data[k][j];
      }
  return Matrix3(newdata);
}


Vector
Matrix3::operator *(const Vector& v) const
{
  double a1,a2,a3,a4,a5,a6,a7,a8,a9;
  a1 = _data[0][0] * v.x();
  a2 = _data[1][0] * v.x();
  a3 = _data[2][0] * v.x();
  a4 = _data[0][1] * v.y();
  a5 = _data[1][1] * v.y();
  a6 = _data[2][1] * v.y();
  a7 = _data[0][2] * v.z();
  a8 = _data[1][2] * v.z();
  a9 = _data[2][2] * v.z();
  
  // We return a homogeneous vector.
  return Vector(a1+a4+a7,
		a2+a5+a8,
		a3+a6+a9 );
  
  
}

Point 
Matrix3::project2D(const Point& p) const {
  double x, y;

  x = _data[0][0] * p.x() + _data[0][1] * p.y() + _data[0][2] * p.z();
  y = _data[1][0] * p.x() + _data[1][1] * p.y() + _data[1][2] * p.z();

  return Point( x, y, 0.0 );
}

Point
Matrix3::operator *(const Point& p) const
{
  double a1,a2,a3,a4,a5,a6,a7,a8,a9;
  a1 = _data[0][0] * p.x();
  a2 = _data[1][0] * p.x();
  a3 = _data[2][0] * p.x();
  a4 = _data[0][1] * p.y();
  a5 = _data[1][1] * p.y();
  a6 = _data[2][1] * p.y();
  a7 = _data[0][2] * p.z();
  a8 = _data[1][2] * p.z();
  a9 = _data[2][2] * p.z();
  
  // We return a homogeneous point.
  return Point(a1+a4+a7,
	       a2+a5+a8,
	       a3+a6+a9 );
  
}


void
Matrix3::identity()
{
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      if (i==j)
	_data[i][j] = 1.0;
      else
	_data[i][j] = 0.0;
}

void
Matrix3::scale(double scale)
{
  _data[0][0] *= scale;
  _data[1][1] *= scale;
  _data[2][2] *= scale;
}

void
Matrix3::scale( const Vector &v ) {
  _data[0][0] *= v.x();
  _data[1][1] *= v.y();
  _data[2][2] *= v.z();
  
}

void
Matrix3::setRow(const Vector& v, int row)
{
  _data[row][0] = v.x();
  _data[row][1] = v.y();
  _data[row][2] = v.z();
}

void
Matrix3::setCol(const Vector& v, int col)
{
  _data[0][col] = v.x();
  _data[1][col] = v.y();
  _data[2][col] = v.z();
}

Matrix3
Matrix3::AlignVectorWithAxis( const Vector &v, AxisType axis ) {
  Matrix3 returnMatrix;
  
  switch ( axis ) {
  case Z_AXIS:
    {
      Vector normal = v;
      Matrix3 xAxis, yAxis;
      xAxis.identity(); yAxis.identity();
      
      double phi = 0.0;
      
      
      // Rotate around X axis
      
      double theta = atan2f( normal.y(), normal.z() );
  
      xAxis._data[1][1] = cos(theta);
      xAxis._data[2][2] = cos(theta);
      xAxis._data[1][2] = sin(-theta);
      xAxis._data[2][1] = sin(theta);
      
      normal = xAxis * normal;
      if ( !((Point)normal).inInterval( Point( 0.0, 0.0, 1.0 ),
					1e-7) ) {
	
	// Rotate around Y axis
	phi = atan2f( normal.x(), normal.z() );
	
	yAxis._data[0][0] = cos(phi);
	yAxis._data[2][2] = cos(phi);
	yAxis._data[0][2] = sin(-phi);
	yAxis._data[2][0] = sin(phi);
	
	normal = yAxis * normal;
      }
      
      returnMatrix = yAxis * xAxis;
      
      // Now, theNormal should be (0,0,1).
      assert( ((Point)normal).inInterval( Point( 0.0, 0.0, 1.0 ),
					  5e-5) );
      
    }
    break;
  case X_AXIS:
  case Y_AXIS:
  default:
    NOT_DONE("NOT YET SUPPROTED");
  }

  return returnMatrix;
}
