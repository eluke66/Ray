
#include "Geometry/Mountains.h"
#include "Geometry/Box.h"
#include "Core/BoundingBox.h"
#include "Core/HitRecord.h"
#include "Core/Math.h"
#include "Core/Point.h"
#include "Core/Ray.h"
#include "Core/Vector.h"
#include "Core/Matrix.h"
#include "Geometry/Group.h"
#include "Core/utils.h"
#include "Core/PerlinNoise.h"
#include "Geometry/QAEB.h"

#include <iostream>
using namespace std;

const double SMOOTH = 0.2625;

class MtnFunc {
public:
  MtnFunc( double xmin, double xmax,
	   double ymin, double ymax,
	   double xOffset, double yOffset,
	   bool falloff) :
    _ymin(ymin), _ymax(ymax),
    _xOffset(xOffset), _yOffset(yOffset), _falloff(falloff) {
    
    _xFactor = (xmax - xmin) * SMOOTH;
    _yFactor = (ymax - ymin) * SMOOTH;
    if ( _falloff )
      _decay = log(0.01) / (ymax - ymin);
  }
  double operator()(double x, double y) const
  {
    double xx = _xOffset + x / _xFactor;
    double yy = _yOffset + y / _yFactor;
    double d = turbulenceAbs( 1, Point(xx, yy,0.5), 2.0, 0.5 );
    
    if ( d < 0.0 )
      d = 0.0;

    if ( _falloff ) {
      double prod = exp(_decay * (y-_ymin));
      if ( prod > 1.0 )
	prod = 1.0;
      d *= prod;
    }
    return d;
  }
  void center(const Point&) {}
private:
  double _xFactor, _yFactor, _decay;
  double _ymin, _ymax;
  double _xOffset, _yOffset;
  bool _falloff;
};

Mountains::Mountains(Material* matl,
		     const Point& p1, const Point& p2,
		     double xOffset, double yOffset,
		     bool falloff)
  : Primitive(matl)
{
  ALLOC;
  int xres = 16;
  double fov = 55;
  double epsilon = 0.5;
  double e = epsilon*tan(fov*M_PI/180/2)/xres;
  _func = new MtnFunc( p1.x(), p2.x(), p1.y(), p2.y(),
		       xOffset, yOffset, falloff );
  _geom = new QAEB<MtnFunc>(matl,
			    p1,
			    p2,
			    e,
			    *_func);
}

Mountains::~Mountains()
{
}

void Mountains::transform(const Matrix4 &matrix) {
  assert(_geom);
  _geom->transform(matrix);
}

double Mountains::valueAt( double x, double y ) const
{
  double val = (*_func)( x, y );
  BoundingBox b;
  _geom->getBounds(b);
  return b.getMin().z() + val * (b.getMax().z() - b.getMin().z());
}

void Mountains::getBounds(BoundingBox& bbox) const
{
  _geom->getBounds(bbox);
}

void Mountains::intersect(HitRecord& hit, const RenderContext&c, const Ray& ray) const
{
  _geom->intersect(hit,c,ray);
}

void Mountains::computeUVW(Vector& uvw, const RenderContext& context,
                           const Ray& ray, const HitRecord& hit) const
{
  _geom->computeUVW( uvw, context, ray, hit );
}

void Mountains::getNormal(Vector& normal, RenderContext&c, const Point& hitpos,
                    const Ray& ray, const HitRecord& hit) const
{
  _geom->normal(normal,c,hitpos,ray,hit);
}
