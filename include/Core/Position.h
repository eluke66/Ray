#ifndef _POSITION_H_
#define _POSITION_H_

#include <iostream>
#include "Core/Vector.h"
#include "Core/BoundingBox.h"
#include "Core/Matrix.h"
class Object;


class Position {
  friend std::istream &operator>>( std::istream &in, Position &p );
public:

  Position();
  ~Position();

  typedef enum { SCALE_MIN, SCALE_MAX, SCALE_REL, SHEAR_REL,
		 SHEAR_ABS, SCALE_INVALID } ScaleType;
  
  void translate( const Vector &to, bool absolute ) {
    _translate = to; _translateAbs = absolute; _useTranslate = true;
  }
  void rotate( const Vector &angles ) {
    _rotateAngles = angles; _useRotate = true;
  }
  void scale( float scale, ScaleType type ) {
    _scale = Vector(scale); _scaleType = type; _useScale = true;
  }
  void shear( const Vector &scale, ScaleType type ) {
    _scale = scale; _scaleType = type; _useScale = true;
  }

  Matrix4 getMatrix( Object *object );
  
  bool transform( Object * object );

  void reset();

  // Returns scale factor, to be used with SCALE_REL
  static double ScaleToFit( Object *object, const BoundingBox &bbox );
  
protected:
  Vector _rotateAngles;
  Vector _translate;
  Vector _scale;

  bool _useRotate, _useTranslate, _useScale;
  bool _translateAbs;

  
  ScaleType _scaleType;
};

#endif

