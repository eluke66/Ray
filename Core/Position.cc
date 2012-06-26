#include "Core/Position.h"
#include "Geometry/Object.h"
#include "Core/Matrix.h"
#include "Core/utils.h"
#include "Core/BoundingBox.h"
#include "Core/Options.h"

Position::Position() : _useRotate(false),
		       _useTranslate(false),
		       _useScale(false)
{
  
}


Position::~Position() {

}

Matrix4
Position::getMatrix(Object *object) {
  assert( object );

  BoundingBox bbox;
  
  float x,y,z;

  // Get the object's dimensions.
  object->getBounds( bbox );
  //if ( !Options::Instance()->quiet())  std::cerr << "Transform: From " << bbox << std::endl;
  Point objectMin = bbox.getMin();
  Point objectMax = bbox.getMax();
  Point objectCenter( (objectMin.x() + objectMax.x()) / 2.0,
		      (objectMin.y() + objectMax.y()) / 2.0,
		      (objectMin.z() + objectMax.z()) / 2.0 );
  
  ///////////////////////////////////////
  //
  // Set up the scale matrix
  //
  ///////////////////////////////////////
  
  Matrix4 scaleMatrix;
  scaleMatrix.identity();
  if ( _useScale ) {
    
    switch ( _scaleType ) {
      
    case SCALE_MIN:
      {
	// Set up the scale matrix such that the object's smallest
	// dimension is the size given in scale.
	float factor = _scale.x() / objectMin.x();
	factor = MIN( factor, _scale.x() / objectMin.y() );
	factor = MIN( factor, _scale.x() / objectMin.z() );
	//	std::cerr << "SCALE FACTOR IS " << factor << std::endl;
	scaleMatrix.scale( Vector( factor ) );
      }
      break;
      
    case SCALE_MAX:
      {
	// Set up the scale matrix such that the object's largest
	// dimension is the size given in scale.
	float factor = _scale.x() / objectMax.x();
	factor = MAX( factor, _scale.x() / objectMax.y() );
	factor = MAX( factor, _scale.x() / objectMax.z() );
	scaleMatrix.scale( Vector( factor ) );
      }
      break;
      
    case SCALE_REL:
    case SHEAR_REL:
      // Just plug in the scale.
      scaleMatrix.scale( _scale );
      break;
      
    case SHEAR_ABS:
      // Adjust the scale such that it takes into account the object
      // size.
      scaleMatrix.scale( Vector( _scale.x() / (objectMax.x() - objectMin.x()),
				 _scale.y() / (objectMax.y() - objectMin.y()),
				 _scale.z() / (objectMax.z() - objectMin.z())));
      break;
      
    case SCALE_INVALID:
    default:
      ERROR("Invalid scale, but we're using it?! BUG!" );
    }
  }

  ///////////////////////////////////////
  //
  // Set up the rotation matrix
  //
  ///////////////////////////////////////
  Matrix4 rotateMatrix;
  rotateMatrix.identity();

  if ( _useRotate ) {
    
    // X
    x = DEGTORAD( _rotateAngles.x() );
    rotateMatrix._data[1][1] = cos(x);
    rotateMatrix._data[2][2] = cos(x);
    rotateMatrix._data[1][2] = sin(x);
    rotateMatrix._data[2][1] = -sin(x);

    // Y
    y = DEGTORAD( _rotateAngles.y() );
    Matrix4 yMat; yMat.identity();
    yMat._data[0][0] = cos(y);
    yMat._data[2][2] = cos(y);
    yMat._data[0][2] = -sin(y);
    yMat._data[2][0] = sin(y);

    // Z
    z = DEGTORAD( _rotateAngles.z() );
    Matrix4 zMat; zMat.identity();
    zMat._data[0][0] = cos(z);
    zMat._data[1][1] = cos(z);
    zMat._data[0][1] = sin(z);
    zMat._data[1][0] = -sin(z);

    rotateMatrix = rotateMatrix * yMat * zMat;
  }
  
  ///////////////////////////////////////
  //
  // Set up the translation matrix
  //
  ///////////////////////////////////////
  Matrix4 translateMatrix;
  translateMatrix.identity();

  if ( _useTranslate ) {
    if ( ! _translateAbs ) {
      // Go to relative space
      // OLD translateMatrix.setCol( _translate - (Vector)objectCenter, 3 );
      translateMatrix.setCol( _translate + (Vector)objectCenter, 3 );
      //std::cerr << "Translating by " << (_translate - (Vector)objectCenter) << std::endl;
      //std::cerr << "\tCenter now is " << objectCenter << std::endl;
    }
    else
      // Go to absolute space
      translateMatrix.setCol( _translate, 3 );
  }
  else
    // Default - go to the object center.
    translateMatrix.setCol( (Vector)objectCenter, 3 );
  

  ///////////////////////////////////////
  //
  // Set up the final composite matrix
  //
  ///////////////////////////////////////

  Matrix4 fullMatrix; fullMatrix.identity();
  
  // Go to origin
  fullMatrix.setCol( Vector( -objectCenter.x(),
			     -objectCenter.y(),
			     -objectCenter.z() ),
		     3 );
  
  // Compose the full transformation
  fullMatrix = translateMatrix * rotateMatrix * scaleMatrix * fullMatrix;

  return fullMatrix;
}


bool
Position::transform( Object * object ) {
  assert( object );

  BoundingBox bbox;
  
  float x,y,z;

  // Get the object's dimensions.
  object->getBounds( bbox );
  //if ( !Options::Instance()->quiet())  std::cerr << "Transform: From " << bbox << std::endl;
  Point objectMin = bbox.getMin();
  Point objectMax = bbox.getMax();
  Point objectCenter( (objectMin.x() + objectMax.x()) / 2.0,
		      (objectMin.y() + objectMax.y()) / 2.0,
		      (objectMin.z() + objectMax.z()) / 2.0 );
  
  ///////////////////////////////////////
  //
  // Set up the scale matrix
  //
  ///////////////////////////////////////
  
  Matrix4 scaleMatrix;
  scaleMatrix.identity();
  if ( _useScale ) {
    
    switch ( _scaleType ) {
      
    case SCALE_MIN:
      {
	// Set up the scale matrix such that the object's smallest
	// dimension is the size given in scale.
	float factor = _scale.x() / objectMin.x();
	factor = MIN( factor, _scale.x() / objectMin.y() );
	factor = MIN( factor, _scale.x() / objectMin.z() );
	//	std::cerr << "SCALE FACTOR IS " << factor << std::endl;
	scaleMatrix.scale( Vector( factor ) );
      }
      break;
      
    case SCALE_MAX:
      {
	// Set up the scale matrix such that the object's largest
	// dimension is the size given in scale.
	float factor = _scale.x() / objectMax.x();
	factor = MAX( factor, _scale.x() / objectMax.y() );
	factor = MAX( factor, _scale.x() / objectMax.z() );
	scaleMatrix.scale( Vector( factor ) );
      }
      break;
      
    case SCALE_REL:
    case SHEAR_REL:
      // Just plug in the scale.
      scaleMatrix.scale( _scale );
      break;
      
    case SHEAR_ABS:
      // Adjust the scale such that it takes into account the object
      // size.
      scaleMatrix.scale( Vector( _scale.x() / (objectMax.x() - objectMin.x()),
				 _scale.y() / (objectMax.y() - objectMin.y()),
				 _scale.z() / (objectMax.z() - objectMin.z())));
      break;
      
    case SCALE_INVALID:
    default:
      ERROR("Invalid scale, but we're using it?! BUG!" );
    }
  }

  ///////////////////////////////////////
  //
  // Set up the rotation matrix
  //
  ///////////////////////////////////////
  Matrix4 rotateMatrix;
  rotateMatrix.identity();

  if ( _useRotate ) {
    
    // X
    x = DEGTORAD( _rotateAngles.x() );
    rotateMatrix._data[1][1] = cos(x);
    rotateMatrix._data[2][2] = cos(x);
    rotateMatrix._data[1][2] = sin(x);
    rotateMatrix._data[2][1] = -sin(x);

    // Y
    y = DEGTORAD( _rotateAngles.y() );
    Matrix4 yMat; yMat.identity();
    yMat._data[0][0] = cos(y);
    yMat._data[2][2] = cos(y);
    yMat._data[0][2] = -sin(y);
    yMat._data[2][0] = sin(y);

    // Z
    z = DEGTORAD( _rotateAngles.z() );
    Matrix4 zMat; zMat.identity();
    zMat._data[0][0] = cos(z);
    zMat._data[1][1] = cos(z);
    zMat._data[0][1] = sin(z);
    zMat._data[1][0] = -sin(z);

    rotateMatrix = rotateMatrix * yMat * zMat;
  }
  
  ///////////////////////////////////////
  //
  // Set up the translation matrix
  //
  ///////////////////////////////////////
  Matrix4 translateMatrix;
  translateMatrix.identity();

  if ( _useTranslate ) {
    if ( ! _translateAbs ) {
      // Go to relative space
      // OLD translateMatrix.setCol( _translate - (Vector)objectCenter, 3 );
      translateMatrix.setCol( _translate + (Vector)objectCenter, 3 );
      //std::cerr << "Translating by " << (_translate - (Vector)objectCenter) << std::endl;
      //std::cerr << "\tCenter now is " << objectCenter << std::endl;
    }
    else
      // Go to absolute space
      translateMatrix.setCol( _translate, 3 );
  }
  else
    // Default - go to the object center.
    translateMatrix.setCol( (Vector)objectCenter, 3 );
  

  ///////////////////////////////////////
  //
  // Set up the final composite matrix
  //
  ///////////////////////////////////////

  Matrix4 fullMatrix; fullMatrix.identity();
  
  // Go to origin
  fullMatrix.setCol( Vector( -objectCenter.x(),
			     -objectCenter.y(),
			     -objectCenter.z() ),
		     3 );
  
  // Compose the full transformation
  fullMatrix = translateMatrix * rotateMatrix * scaleMatrix * fullMatrix;

  //  std::cerr << "full is " << fullMatrix << std::endl;
  // And transform the geometry
  object->transform( fullMatrix );
  object->getBounds( bbox );
  //  if ( !Options::Instance()->quiet())  std::cerr << "Transform: To " << bbox << std::endl;

  if ( _useTranslate && _translateAbs ) {
    Point objectMin = bbox.getMin();
    Point objectMax = bbox.getMax();
    Point objectCenter( (objectMin.x() + objectMax.x()) / 2.0,
			(objectMin.y() + objectMax.y()) / 2.0,
			(objectMin.z() + objectMax.z()) / 2.0 );
    //    std::cerr << "Center: " << objectCenter << " translate" << _translate << std::endl;
    //    assert( objectMin.inInterval( (Point)_translate, 1e-3 ) );
  }

  return true;
}

void
Position::reset() {
  _rotateAngles = _translate = _scale = Vector(0,0,0);
  _useRotate = _useTranslate = _useScale = _translateAbs = false;  
}

double
Position::ScaleToFit( Object * object, const BoundingBox &bbox ) {
  assert(object);
  
  BoundingBox b2;
  object->getBounds(b2);

  Vector bsize = bbox.getMax() - bbox.getMin();

  Vector osize = b2.getMax() - b2.getMin();

  Vector sizes = bsize / osize;
  return sizes.minComponent();
}



std::istream &
operator>>( std::istream &in, Position &p ) {
#if 0
  // positional stuff is:
  //
  // translate <abs/rel> <point> 
  // rotate <vector>
  // scale min/max <value> (scale ss that min/max dimension is that val).
  // scale rel     <value> (scale all axes by given value)
  // shear rel/abs <vector> (scale axes by different values).

  char brace;
  std::string input;
  
  in >> brace;
  
  READLN( input, in );

  while ( input != "}" ) {
    if ( input == "translate" ) {
      
      // Read translate stuff.
      READLN(input, in);
      if ( input == "abs" )
	p._translateAbs = true;
      else if ( input == "rel" )
	p._translateAbs = false;
      else
	ERROR("Unknown translation mode: " + input );
      in >> p._translate;
      
      p._useTranslate = true;
    }
    else if ( input == "rotate" ) {

      // Do rotation
      in >> p._rotateAngles;
      p._useRotate = true;
    }
    else if ( input == "scale" ) {

      // Do scale stuff
      READLN(input,in);
      if ( input == "min" ) {
	p._scaleType = Position::SCALE_MIN;
      }
      else if ( input == "max" ) {
	p._scaleType = Position::SCALE_MAX;
      }
      else if ( input == "rel" ) {
	p._scaleType = Position::SCALE_REL;
      }
      else {
	ERROR( "Unknown scale type: " + input );
	p._scaleType = Position::SCALE_INVALID;
      }

      float f;
      in >> f;

      // If the scale type is not invalid, use the scale
      if ( p._scaleType != Position::SCALE_INVALID ) {
	p._scale = Vector(f);
	p._useScale = true;
      }
    }
    else if ( input == "shear" ) {

      // Do shear (per-axis scaling)
      READLN(input,in);
      if ( input == "rel" ) {
	p._scaleType = Position::SHEAR_REL;
      }
      else if (input == "abs" ) {
	p._scaleType = Position::SHEAR_ABS;
      }
      else {
	ERROR( "Unknown shear type: " + input );
	p._scaleType = Position::SCALE_INVALID;
      }

      in >> p._scale;
      if ( p._scaleType != Position::SCALE_INVALID )
	p._useScale = true;
      
    }
    else {
      ERROR( "Invalid option to position: " + input );
    }
    
    READLN(input, in);
  }
#endif  
  return in;
}
