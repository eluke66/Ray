#ifndef GradientColor_h
#define GradientColor_h

class HitRecord;
class Ray;
class RenderContext;
class Vector;

#include "Core/Color.h"

class GradientColor {
public:
  // Horizontal gradient by default
  GradientColor( const Color& tr, const Color &tl, bool horizontal=true,
		 float repeat=1.0) 
    : _tr(tr), _tl(tl), _br(tr), _bl(tl), _hRepeat(repeat), _vRepeat(1.0)
  {
    if ( !horizontal ) {
      // Swap to vertical gradient
      _tl = tr;
      _br = tl;
      _bl = tl;
      _vRepeat = repeat;
      _hRepeat = 1.0;
    }
  }
    
  // 4 Corners
  GradientColor( const Color& tr, const Color &tl, const Color& br,
		 const Color &bl, float hRepeat, float vRepeat ) :
    _tr(tr), _tl(tl), _br(br), _bl(bl), _hRepeat(hRepeat),
    _vRepeat(vRepeat) {}
  
  void operator()(Color& result, RenderContext& context, const Ray& ray,
		  const HitRecord& hit, const Color& atten, int depth) const;
  
  void operator()(Color& result, const Vector &uvw ) const;
private:
  Color _tr, _tl, _br, _bl;
  float _hRepeat, _vRepeat;
  GradientColor();
};

#include <list>
class MultiGradientColor {
public:
  MultiGradientColor( bool horizontal = true )
    : _horizontal( horizontal ) {}
  void addPoint( double point, const Color &color );
  void operator()(Color& result, RenderContext& context, const Ray& ray,
		  const HitRecord& hit, const Color& atten, int depth) const;
  
  void operator()(Color& result, const Vector &uvw ) const;
  
protected:
  struct PColor {
    PColor( double pt, const Color &color ) :
      _pt(pt), _color(color) {}
    double _pt;
    Color  _color;
  };
  typedef std::list<PColor> GradientList;
  GradientList _colors;
  bool _horizontal;
};

#include "Core/Point.h"
#include <vector>
class MultiGradientColor2D {
public:
  MultiGradientColor2D(double decay=10) : _decay(decay)
  {
    assert( _decay > 0);
  }
  void addPoint( const Point2D &point, const Color &color );
  void operator()(Color& result, RenderContext& context, const Ray& ray,
		  const HitRecord& hit, const Color& atten, int depth) const;
  
  void operator()(Color& result, const Vector &uvw ) const;
  
protected:
  struct PColor {
    PColor( const Point2D &pt, const Color &color ) :
      _pt(pt), _color(color) {}
    Point2D _pt;
    Color   _color;
  };
  typedef std::vector<PColor> GradientVector;
  GradientVector _colors;
  double         _decay;

};

  
#endif
