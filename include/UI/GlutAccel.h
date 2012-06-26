#ifndef _GLUT_ACCEL_H_
#define _GLUT_ACCEL_H_

#include <vector>

class Geometry;
class Color;
class GlutAccelerator;

class GlutAccel {
  friend class UI;
  friend class GlutAccelerator;
public:
  static const Geometry * GetHit( int x, int y );
  
private:
  GlutAccel();
  ~GlutAccel();

  static void SetupAccel(unsigned w, unsigned h);
  static void RenderAccel();
  static void TeardownAccel();
  static unsigned char * GrabScreenMap();
  
  static unsigned char * _screenMap;
  static unsigned _width;
  static unsigned _height;

  typedef std::vector<const Geometry *> ObjectVector;
  static ObjectVector _colorLookupTable;

  static Color _renderColor;
};

#endif
