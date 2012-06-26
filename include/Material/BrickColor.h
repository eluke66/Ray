#ifndef BrickColor_h
#define BrickColor_h

#include "Core/Color.h"
#include "Core/Point.h"
#include "Core/Vector.h"

class Material;
class HitRecord;
class Ray;
class RenderContext;

/*
  Color range of brick.
  Style of brick (all aligned, offset, etc)
  Size of brick
  Thickness of mortar.
  Color of mortar.
*/
class BrickColor {
public:
  BrickColor(const Color &light, const Color &dark,
	     bool aligned, const Point2D &brickSize,
	     const Color &mortarColor,
	     double uRepeat = 1.0, double vRepeat = 1.0,
	     bool constantColor = true);
  void operator()(Color& result, RenderContext& context, const Ray& ray,
		  const HitRecord& hit, const Color& atten, int depth) const;

  inline bool aligned() const { return _aligned; }
  inline const Point2D& brickSize() const { return _brickSize; }
  inline double uRepeat() const { return _uRepeat; }
  inline double vRepeat() const { return _vRepeat; }
private:
  Color   _light, _dark;
  bool    _aligned;
  Point2D _brickSize;
  Color   _mortarColor;
  double  _uRepeat, _vRepeat;
  bool    _constantColor;
  
  BrickColor();
  BrickColor &operator=(const BrickColor&);
};

#endif
