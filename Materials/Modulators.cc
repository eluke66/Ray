#include "Material/Modulators.h"

#include "Core/HitRecord.h"
#include "Core/Ray.h"
#include "Core/Vector.h"
#include "Core/utils.h"
#include "Core/Image.h"
#include "Geometry/Primitive.h"
#include <iostream>

CheckerModulator::CheckerModulator(const Point& origin, const Vector& v1, const Vector& v2)
  : _origin(origin), _v1(v1), _v2(v2), _v3(0,0,0)
{
  _v1 *= 1./v1.length2();
  _v2 *= 1./v2.length2();
}

void CheckerModulator::getWeights( std::vector<double> &weights,
				   RenderContext& context,
				   const Ray& ray,
				   const HitRecord& hit ) const {
  Vector uvw;
  hit.getPrimitive()->computeUVW(uvw, context, ray, hit);
  int i1 = Floor(Dot(uvw, _v1));
  int i2 = Floor(Dot(uvw, _v2));
  int which_matl = (i1+i2)%2;
  weights.resize(2);
  weights[ 1 - which_matl ] = 0.0;
  weights[ which_matl ] = 1.0;
}

void GradientModulator::getWeights( std::vector<double> &weights,
				    RenderContext& context,
				    const Ray& ray,
				    const HitRecord& hit ) const {
  Vector uvw;
  hit.getPrimitive()->computeUVW(uvw, context, ray, hit);
  double x = fabs(uvw.x() * _hRepeat);

  mdebug("UVW: " << uvw << " x is " << x);
  if ( x > 1.0 ) {
    x -= trunc(x);
  }
  double y = fabs(uvw.y() * _vRepeat);
  if ( y > 1.0 ) {
    y -= trunc(y);
  }
    
  assert( IN_RANGE(x, 1.0, 0.0, 1e-6) );
  assert( IN_RANGE(y, 1.0, 0.0, 1e-6) );

  weights.push_back( (1.0-x)*(1.0-y) );
  weights.push_back( (1.0-y) );
  weights.push_back( (1.0-x)*y );
  weights.push_back( x * y );
}


void TextureModulator::getWeights( std::vector<double> &weights,
				   RenderContext& context,
				   const Ray& ray,
				   const HitRecord& hit ) const {
  assert( _image );
  
  Vector uvw;
  hit.getPrimitive()->computeUVW(uvw, context, ray, hit);
  double x = uvw.x() * _uRepeat;
  double y = uvw.y() * _vRepeat;

  Color result = _image->interpolate(x - floor(x),
				     y - floor(y));

  // For now, we default to modulating between two materials.
  weights.resize(2);
  weights[0] = (result.maxComponent() > 0.5) ? 1.0 : 0.0;
  weights[1] = 1.0 - weights[0];
}


SimpleBuildingModulator::SimpleBuildingModulator( double hGap, double vGap,
						  double hRepeat, double vRepeat) :
  _hGap(hGap/2.0), _vGap(vGap/2.0), _hRepeat(hRepeat), _vRepeat(vRepeat)
{
  assert( _hGap >= 0.0 && _hGap <= 0.5 );
  assert( _vGap >= 0.0 && _vGap <= 0.5 );
  assert( _hRepeat > 0.0 );
  assert( _vRepeat > 0.0 );
}

void
SimpleBuildingModulator::getWeights( std::vector<double> &weights,
				     RenderContext &context,
				     const Ray &ray,
				     const HitRecord &hit ) const {
  // Get uvw.
  Vector uvw;
  hit.getPrimitive()->computeUVW( uvw, context, ray, hit );
  double x = uvw.x() * _hRepeat;
  double y = uvw.y() * _vRepeat;
  x -= trunc(x);
  y -= trunc(y);
  
  // Figure out which material we've got.
  int whichMatl = 0; // Default is non-window
  if ( x > _hGap &&
       x < (1 - _hGap) &&
       y > _vGap &&
       y < (1 - _vGap) )
    whichMatl = 1;
  
  // Compute weights
  weights.resize(2);
  weights[ 1 - whichMatl ] = 0.0;
  weights[ whichMatl ] = 1.0;
}

void CityGridModulator::addMaterial( CityMat material,
				     const Point2D &min, const Point2D &max )
{
  // Add to global list
  BoundingBox2D matB( min, max );
  _matList.push_back( bbmat(matB,material)  );

  unsigned index = _matList.size() - 1;

  // Figure out grid locations
  double dx = (_max.x() - _min.x()) / _N;
  double dy = (_max.y() - _min.y()) / _N;
  
  for ( int i = 0; i < _N; ++i ) {
    for ( int j = 0; j < _N; ++j ) {
      BoundingBox2D b( _min + Point2D(i*dx, j*dy),
		       _min + Point2D((i+1)*dx, (j+1)*dy) );
      if ( matB.overlaps(b) ) {
	_grid[i][j].push_back(index);
      }
    }
  }

}
  
void CityGridModulator::getWeights( std::vector<double> &weights,
				    RenderContext &context,
				    const Ray &ray,
				    const HitRecord &hit ) const
{
  // For each material in that grid, see if it's a hit.
  weights.resize(5);
  for ( int i = 0; i < 5; ++i )
    weights[i] = 0.0;


  // Find the hitpoint.
  Point hitpoint = ray.origin() + hit.minT() * ray.direction();

  if ( hitpoint.x() < _min.x() ||
       hitpoint.x() > _max.x() ||
       hitpoint.y() < _min.y() ||
       hitpoint.y() > _max.y() )
    {
      weights[0] = 1.0;
      return;
    }
  double dx = (_max.x() - _min.x()) / _N;
  double dy = (_max.y() - _min.y()) / _N;
  
  // Figure out which grid it's in.
  // hit = min + Q*dx
  // Q = hit-min/dx
  int x = int((hitpoint.x() - _min.x()) / dx);
  int y = int((hitpoint.y() - _min.y()) / dy);
  assert( y < _N && y >= 0 && x < _N && x >= 0 );

  
  const std::vector<unsigned> &matlist = _grid[x][y];
  bool defaultMaterial = true;
  for ( unsigned i = 0; i < matlist.size(); ++i ) {
    if ( _matList[ matlist[i] ].box.inside( (Point2D)hitpoint ) ) {
      weights[ _matList[ matlist[i] ].mat ] = 1.0;
      defaultMaterial = false;
      break;
    }
  }

  if ( defaultMaterial )
    weights[0] = 1.0;
}

void SplitModulator::getWeights( std::vector<double> &weights,
				 RenderContext& context,
				 const Ray& ray,
				 const HitRecord& hit ) const {
  
  Vector uvw;
  hit.getPrimitive()->computeUVW(uvw, context, ray, hit);

  ////////////////////////////////
  // This is dumb!
  BoundingBox bbox;
  _obj->getBounds(bbox);
  ///////////////////////////////
  double where;
  double bmin, bmax;

  switch ( _split ) {
  case XSPLIT:
    where = uvw.x();
    bmin = bbox.getMin().x();
    bmax = bbox.getMax().x();
    break;
  case YSPLIT:
    where = uvw.y();
    bmin = bbox.getMin().y();
    bmax = bbox.getMax().y();
    break;
  case ZSPLIT:
    where = uvw.z();
    bmin = bbox.getMin().z();
    bmax = bbox.getMax().z();
    break;
  default:
    assert(!"Unknown kind of split!");
  }

  // Convert 'where' to range of min (0.0) to max (1.0)
  where = (where - bmin) / (bmax - bmin);
  where *= _repeat;
  where -= floor(where);
  
  // For now, we default to modulating between two materials.
  weights.resize(2);
  
  if ( where > _boundary ) {
    weights[0] = 0.0;
    weights[1] = 1.0;
  }
  else {
    weights[0] = 1.0;
    weights[1] = 0.0;
  }
}
