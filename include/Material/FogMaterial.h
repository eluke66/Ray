#ifndef Fog_Material_h
#define Fog_Material_h

class Color;
class RenderContext;
class Ray;
class HitRecord;
class BoundingBox;
class Point;

#include "Core/Vector.h"
#include "Core/Color.h"

/***********************************************/
class FogCalculator {
public:
  virtual ~FogCalculator() {}
  virtual double valueAt( const Point &point,
			  const BoundingBox &b ) const = 0;
    
    
};
/***********************************************/
class ConstantFogCalculator : public FogCalculator {
public:
  ConstantFogCalculator( double opacity ) : _opacity(opacity) {}
  virtual double valueAt( const Point &,
			  const BoundingBox & ) const { return _opacity; }
private:
  double _opacity;
};
/***********************************************/
class PerlinFogCalculator : public FogCalculator {
public:
  PerlinFogCalculator( int octaves, double lacunarity, double gain ) :
    _octaves(octaves), _lacunarity(lacunarity), _gain(gain) {}
  virtual double valueAt( const Point &point,
			  const BoundingBox &b ) const;
private:
  int _octaves;
  double _lacunarity, _gain;
};

/***********************************************/
class GradientFogCalculator : public FogCalculator {
public:
  GradientFogCalculator( const Vector &falloffDir,
			 double startOpacity,
			 double finalOpacity=0.0) :
    _falloff(falloffDir), _start(startOpacity), _end(finalOpacity) {_falloff.normalize();}
  virtual double valueAt( const Point &point,
			  const BoundingBox &b ) const = 0;
protected:
  GradientFogCalculator();
  double findGradientSpot(const Point &point,
			  const BoundingBox &b ) const;
  Vector _falloff;
  double _start, _end;
};

class LinearFogCalculator : public GradientFogCalculator {
public:
  LinearFogCalculator( const Vector &falloffDir,
		       double startOpacity,
		       double finalOpacity=0.0) :
    GradientFogCalculator(falloffDir, startOpacity, finalOpacity) {}
  virtual double valueAt( const Point &point,
			  const BoundingBox &b ) const;
};

class ExpFogCalculator : public GradientFogCalculator {
public:
  ExpFogCalculator( double falloffK,
		    const Vector &falloffDir,
		    double startOpacity,
		    double finalOpacity=0.0) :
    GradientFogCalculator(falloffDir, startOpacity, finalOpacity),
    _falloffK(falloffK) {}
  virtual double valueAt( const Point &point,
			  const BoundingBox &b ) const;
private:
  double _falloffK;
};

class GaussianFogCalculator : public GradientFogCalculator {
   GaussianFogCalculator( double stddev,
			  const Vector &falloffDir,
			  double startOpacity,
			  double finalOpacity=0.0) :
     GradientFogCalculator(falloffDir, startOpacity, finalOpacity),
     _stddev(stddev) {}
  virtual double valueAt( const Point &point,
			  const BoundingBox &b ) const;
private:
  double _stddev;
};

/***********************************************/
class FogCombiner : public FogCalculator {
public:
  FogCombiner( FogCalculator *one, FogCalculator *two,
	       double weightOne = 1.0, double weightTwo = 1.0) :
    _one(one), _two(two), _weightOne(weightOne), _weightTwo(weightTwo) {}
  virtual double valueAt( const Point &point,
			  const BoundingBox &b ) const {
    return (_weightOne * _one->valueAt( point,b )) + (_weightTwo * _two->valueAt( point, b ));
  }
private:
  FogCalculator *_one, *_two;
  double _weightOne, _weightTwo;
};

/***********************************************/
/***********************************************/
class FogMaterial {
public:
  FogMaterial( double opacityScale, const Color &color, int numSamples,
	       FogCalculator *calc);
  virtual ~FogMaterial();
  void operator()(Color& result, RenderContext& context, const Ray& ray,
		  const HitRecord& hit, const Color& atten, int depth) const;
private:
  FogMaterial();
  double _scale;
  Color  _color;
  int    _numSamples;
  FogCalculator *_calc;
};

#include "Material/Material.h"

typedef _Material<FogMaterial> _FogMaterial;

#define Fog(a,b,c,d)				\
  _FogMaterial(FogMaterial(a,b,c,d))
#endif
