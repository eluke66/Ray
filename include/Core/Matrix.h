#ifndef _MATRIX_H_
#define _MATRIX_H_

#include <iostream>

class Vector;
class Point;
class Matrix3;

class Matrix4 {
  friend std::ostream& operator<<(std::ostream& o, const Matrix4& m);
public:
  Matrix4();
  Matrix4(const Matrix4& m);
  Matrix4(const Matrix3& m);
  Matrix4(double data[4][4]);
  ~Matrix4();

  Matrix4 operator *(const Matrix4& m) const;
  Vector operator *(const Vector& v) const;
  Point  operator *(const Point& p) const;

  // Like *, but without translation.
  Vector transform(const Vector &v ) const;


  void identity();
  bool isIdentity() const;
  void scale(double scale);
  void scale( const Vector &v );
  void setRow(const Vector& v, int row);
  void setCol(const Vector& v, int col);

  double determinant() const;

  Matrix4 invert() const;
  
  double _data[4][4];
};


class Matrix3 {
  friend std::ostream& operator<<(std::ostream& o, const Matrix3& m);
public:
  Matrix3();
  Matrix3(const Matrix3& m);
  Matrix3(const Matrix4& m);
  Matrix3(double data[3][3]);
  ~Matrix3();

  Matrix3 operator *(const Matrix3& m) const;
  Vector operator *(const Vector& v) const;
  Point  operator *(const Point& p) const;

  // Ignores z component.
  Point project2D(const Point& p) const;
  void identity();
  void scale(double scale);
  void scale( const Vector &v );
  void setRow(const Vector& v, int row);
  void setCol(const Vector& v, int col);

  enum AxisType { X_AXIS, Y_AXIS, Z_AXIS };
  static Matrix3 AlignVectorWithAxis( const Vector &vector, AxisType axis );
				      
  double _data[3][3];

};

#endif
