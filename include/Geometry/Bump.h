#ifndef Bump_h_
#define Bump_h_

#include "Core/Vector.h"
#include "Core/Color.h"
#include "Core/utils.h"

class Bump {
public:
  virtual ~Bump() {}

  virtual Vector getBump( RenderContext &context,
			  const Ray &ray,
			  const HitRecord &hit ) const = 0;
};


class ConstantBump : public Bump {
public:
  ConstantBump( const Vector &value ) : _value(value) {}
  virtual Vector getBump( RenderContext &,
			  const Ray &,
			  const HitRecord &) const { return _value; }
private:
  Vector _value;
};

class RandomBump : public Bump {
public:
  RandomBump( double value ) : _value(value) {}
  virtual Vector getBump( RenderContext &,
			  const Ray &ray,
			  const HitRecord &hit) const;
private:
  double   _value;
};


const float DIFF_COLOR = 128.0 / 255.0;

template <typename ColorClass>
class UVWBump : public Bump {
public:
  UVWBump( const ColorClass &colorizer ) :
    _colorizer( colorizer ) {}

  virtual Vector getBump( RenderContext &context,
			  const Ray &ray,
			  const HitRecord &hit ) const {
    Color result;
    _colorizer(result, context, ray, hit, Color(1,1,1), 0);

    return Vector(result.r() - DIFF_COLOR,
		  result.g() - DIFF_COLOR,
		  result.b() - DIFF_COLOR) * 2.0;
  }
private:
  ColorClass _colorizer;
};

class GradientColor;
class TextureColor;
class CheckerColor;
class WoodColor;
class MarbleColor;

typedef UVWBump<GradientColor> GradientBump;
typedef UVWBump<TextureColor>  MapBump;
typedef UVWBump<CheckerColor>  CheckerBump;
typedef UVWBump<WoodColor>     WoodBump;
typedef UVWBump<MarbleColor>   MarbleBump;

class Image;
Image * HeightfieldToBumpMap( const Image &in );


class BrickColor;
Bump * MakeBrickBump( const BrickColor &brick );

#endif
