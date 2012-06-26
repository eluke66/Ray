#include "Material/Material.h"
#include "Material/BrickColor.h"

#include "Core/HitRecord.h"
#include "Core/Vector.h"
#include "Core/Ray.h"
#include "Geometry/Primitive.h"
#include "Core/PerlinNoise.h"
#include "Core/utils.h"

BrickColor::BrickColor(const Color &light, const Color &dark,
		       bool aligned, const Point2D &brickSize,
		       const Color &mortarColor,
		       double uRepeat, double vRepeat,
		       bool constantColor)
  : _light(light), _dark(dark), _aligned(aligned),
    _brickSize(brickSize),
    _mortarColor(mortarColor),
    _uRepeat(uRepeat), _vRepeat(vRepeat),
    _constantColor(constantColor)
    
{
}

void BrickColor::operator()(Color& result, RenderContext& context,
			    const Ray& ray, const HitRecord& hit,
			    const Color& atten, int depth) const
{
  Vector uvw;
  hit.getPrimitive()->computeUVW(uvw, context, ray, hit);
  double u = uvw.x() * _uRepeat;
  double v = uvw.y() * _vRepeat;
  
  // If aligned, then row is immaterial 
  
  // Figure out if we're in brick or mortar.
  if ( ! _aligned ) {
    if ( (int)floor(v) % 2 == 0 )
      u -= 0.5;
  }
  double whichBrickU = floor(u) * 13.7;
  double whichBrickV = floor(v) * 4.9;
  u -= floor(u);
  if ( u < 0 )
    u += 1.0;
  v -= floor(v);
  
  // If brick if u < bricksize && v < bricksize 
  bool brick = ( u < _brickSize.x() && v < _brickSize.y() );

  
  if ( brick ) {
    Point p( ray.origin() + hit.minT() * ray.direction() );
    double amt;
    if ( _constantColor )
      amt = (perlinNoise( Point( whichBrickU, whichBrickV, 0 )) + 1) * 0.5;
    else
      amt = perlinNoise( p );

    amt = clamp(amt);
    result = Color::Lerp( _light, _dark, amt );
  }
  else
    result = _mortarColor;
}
