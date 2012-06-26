#ifndef Blur_h_
#define Blur_h_

#include "Core/Vector.h"
#include "Core/Color.h"
class Blur {
public:
  virtual ~Blur() {}

  virtual double getBlur( RenderContext &context,
			  const Ray &ray,
			  const HitRecord &hit ) const = 0;
};


class ConstantBlur : public Blur {
public:
  ConstantBlur( double value ) : _value(value) {}
  virtual double getBlur( RenderContext &,
			  const Ray &,
			  const HitRecord &) const { return _value; }
private:
  double _value;
};

template <typename ColorClass>
class UVWBlur : public Blur {
public:
  UVWBlur( const ColorClass &colorizer ) :
    _colorizer( colorizer ) {}

  virtual double getBlur( RenderContext &context,
			  const Ray &ray,
			  const HitRecord &hit ) const {
    Color result;
    _colorizer(result, context, ray, hit, Color(1,1,1), 0);
    return result.maxComponent();
  }
private:
  ColorClass _colorizer;
};

#if 0
class GradientBlur : public Blur {
public:
  GradientBlur( const GradientColor &gradient ) :
    _gradient(gradient) {}
  virtual double getBlur( const Vector &uvw ) const {
    Color c;
    _gradient( c, uvw );
    return c.maxComponent();

  }
private:
  GradientColor _gradient;
};

class MapBlur : public Blur {
public:
  MapBlur( const TextureColor &map ) :
    _map(map) {}
  virtual double getBlur( const Vector &uvw ) const {
    Color c;
    _map( c, uvw );
    return c.maxComponent();
  }
private:
  TextureColor _map;
};
#else

class GradientColor;
class TextureColor;
class CheckerColor;
class WoodColor;
class MarbleColor;

typedef UVWBlur<GradientColor> GradientBlur;
typedef UVWBlur<TextureColor>  MapBlur;
typedef UVWBlur<CheckerColor>  CheckerBlur;
typedef UVWBlur<WoodColor>     WoodBlur;
typedef UVWBlur<MarbleColor>   MarbleBlur;

#endif

#endif
