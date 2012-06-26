#include "Material/GradientColor.h"

#include "Core/HitRecord.h"
#include "Core/Vector.h"
#include "Geometry/Primitive.h"
#include "IO/ImageLoader.h"
#include <iostream>
#include "Core/utils.h"

void GradientColor::operator()(Color& result, RenderContext& context, const Ray& ray,
			      const HitRecord& hit, const Color& atten, int depth) const {

  Vector uvw;
  hit.getPrimitive()->computeUVW(uvw, context, ray, hit);

  (*this)(result, uvw);
  
}

void
GradientColor::operator()(Color& result, const Vector &uvw ) const {
  double x = uvw.x() * _hRepeat;
  while ( x > 1.0 ) x -= 1.0;
  double y = uvw.y() * _vRepeat;
  while ( y > 1.0 ) y -= 1.0;

  assert( IN_RANGE(x, 1.0, 0.0, 1e-6) );
  assert( IN_RANGE(y, 1.0, 0.0, 1e-6) );

  result = _tl*(1.0-x)*(1.0-y) + _tr*x*(1.0-y) + _bl*(1.0-x)*y + _br * x * y;
}

void
MultiGradientColor::addPoint( double point, const Color &color ) {

  // Make sure it's in range of 0-1
  assert( point <= 1.0 );
  assert( point >= 0.0 );

  // Insert it into the list, sorted.
  GradientList::iterator it;
  GradientList::iterator location = _colors.begin();
  for ( it = _colors.begin(); it != _colors.end(); ++it ) {
    ++location;
    if ( it->_pt > point )
      break;
  }

  _colors.insert( location, PColor( point, color ) );
}


void 
MultiGradientColor::operator()(Color& result, RenderContext& context,
			       const Ray& ray,
			       const HitRecord& hit, const Color& atten,
			       int depth) const {

  Vector uvw;
  assert(hit.getPrimitive());
  hit.getPrimitive()->computeUVW( uvw, context, ray, hit );
  (*this)(result, uvw );
}

void 
MultiGradientColor::operator()(Color& result, const Vector &uvw ) const
{
  // Find the two points surrounding the UVW.
  double point = uvw.y();
  
  if ( _horizontal )
    point = uvw.x();

  if ( _colors.begin()->_pt >= point ) {
    result = _colors.begin()->_color;
    return;
  }

  GradientList::const_iterator current = _colors.begin();
  GradientList::const_iterator next = _colors.begin();
  ++next;
  
  while (next != _colors.end()) {
    if (current->_pt <= point &&
	(next == _colors.end() || 
	 next->_pt >= point) ) {
      break;
    }
    
    ++current;
    ++next;
  } // End of while loop
  
  // If next is off the end, then we get all current.
  double cPart, nPart;
  if ( next == _colors.end() ) {
    cPart = 1.0;
    nPart = 0.0;
  }
  else {
    nPart = (point - current->_pt) / (next->_pt - current->_pt);
    cPart = 1.0 - nPart;
  }

  result = next->_color * nPart + current->_color * cPart;
  
}



void
MultiGradientColor2D::addPoint( const Point2D &point, const Color &color ) {

  // Make sure it's in range of 0-1
  assert( point.x() <= 1.0 );
  assert( point.x() >= 0.0 );
  assert( point.y() <= 1.0 );
  assert( point.y() >= 0.0 );

  // Insert it into the list.
  _colors.push_back( PColor( point, color ) );
}


void 
MultiGradientColor2D::operator()(Color& result, RenderContext& context,
				 const Ray& ray,
				 const HitRecord& hit, const Color& atten,
				 int depth) const {
  
  Vector uvw;
  assert(hit.getPrimitive());
  hit.getPrimitive()->computeUVW( uvw, context, ray, hit );
  (*this)(result, uvw );
}

void 
MultiGradientColor2D::operator()(Color& result, const Vector &uvw ) const
{
  Point2D point(uvw.x() - floor(uvw.x()),
		uvw.y() - floor(uvw.y()));
  
  // Find the distance from each point to the 2D location,
  // also adding the total distance.
  typedef std::map< const PColor*, double > DistMap;
  DistMap distMap;
  for ( unsigned i = 0; i < _colors.size(); ++i ) {
    Vector2D dist = _colors[i]._pt - point;
    double dl = dist.length2();
    distMap[ &_colors[i] ] = dl;
  }

  // For each point, add the contribution to the result.
  result = Color(0.0);
  double totalWeight = 0.0;
  for ( DistMap::iterator it = distMap.begin();
	it != distMap.end();
	++it ) {

    double value = _decay*it->second;
    double contribution;
    if ( value > 30 )
      contribution = 0;
    else 
      contribution = exp(-value);
    totalWeight += contribution;
    Color impact = (it->first)->_color * contribution;
    result += impact;
    
  }

  if ( totalWeight > 0.0 ) {
    result /= totalWeight;
  }
  else {
    PRINT_ONCE("\n\n***ERROR*** Too high of a decay in MultiGradientColor2D: " << _decay);
  }
}
