
#include "Geometry/Cylinder.h"
#include "Core/BoundingBox.h"
#include "Core/HitRecord.h"
#include "Core/Math.h"
#include "Core/Ray.h"
#include "Core/Matrix.h"
#include "Core/utils.h"

Cylinder::Cylinder(Material* material, double height, double radius)
  : Primitive(material), _height(height), _radius(radius)
{
  ALLOC;
  _matrix.identity();
  _mInverse.identity();
}

Cylinder::~Cylinder()
{
}

Ray transformRay( const Ray &ray, const Matrix4& _mInverse ) {
  Point pTrans = _mInverse * ray.origin();
  Vector vTrans = _mInverse.transform(ray.direction());
  vTrans.normalize();
  return Ray( pTrans, vTrans );
}

void Cylinder::transform(const Matrix4 &matrix) {
  _matrix = matrix;
  _mInverse = _matrix.invert();
}

void Cylinder::getBounds(BoundingBox& bbox) const
{
  if ( !_matrix.isIdentity()) {
    BoundingBox b;
    Point p[8];
    b.extend(Point( -_radius, -_radius, 0));
    b.extend(Point(_radius, _radius, _height));
    p[1] = Point(b.getMin().x(), b.getMin().y(), b.getMin().z());
    p[2] = Point(b.getMax().x(), b.getMin().y(), b.getMin().z());
    p[3] = Point(b.getMax().x(), b.getMax().y(), b.getMin().z());
    p[4] = Point(b.getMin().x(), b.getMax().y(), b.getMin().z());
    p[5] = Point(b.getMin().x(), b.getMin().y(), b.getMax().z());
    p[6] = Point(b.getMax().x(), b.getMin().y(), b.getMax().z());
    p[7] = Point(b.getMax().x(), b.getMax().y(), b.getMax().z());
    p[0] = Point(b.getMin().x(), b.getMax().y(), b.getMax().z());
    
    for ( int i = 0; i < 8; ++i )
      p[i] = _matrix * p[i];
    
    for ( int i = 0; i < 8; ++i )
      bbox.extend( p[i] );
  }
  else{
    bbox.extend(Point( -_radius, -_radius, 0));
    bbox.extend(Point(_radius, _radius, _height));
  }
}

void Cylinder::intersect(HitRecord& hit, const RenderContext& context, const Ray& ray) const
{
  Ray tRay = transformRay( ray, _mInverse );
  
  intersectI(hit,context,tRay);
}

void Cylinder::intersectI(HitRecord& hit, const RenderContext& context, const Ray& ray) const
{
  
  Vector direction = ray.direction();
  direction.normalize();
  Point origin = ray.origin();

  double rdx = direction.x();
  double rdy = direction.y();
  double rad = sqrt(rdx*rdx + rdy*rdy);
  rdx = rdx / rad;
  rdy = rdy / rad;
  double rox = origin.x();
  double roy = origin.y();
  
  double a = rdx * rdx + rdy*rdy;
  double b = 2.0 * ( rox*rdx + roy*rdy );
  double c = rox*rox + roy*roy - ( _radius * _radius );
  double sqr =  b*b - 4.0*a*c;

  if ( sqr >= 0.0 ) {
    
    double t1 = (-b + sqrt(sqr)) / 2.0 * a;
    double t2 = (-b - sqrt(sqr)) / 2.0 * a;

    double z = origin.z() + t1 * direction.z() / rad;
    if ( t1 >= 0.0 && z >= 0.0 && z <= _height ) {
      hit.hit(t1/rad, this, matl);
    }
    
    z = origin.z() + t2 * direction.z() / rad;
    if ( t2 >= 0.0 && z >= 0.0 && z <= _height ) {
      hit.hit(t2/rad, this, matl);
    }
  }

  // Now check the caps
#ifndef NO_CAP
  // Top
  if ( Abs(ray.direction().z()) > 1e-6 ) {
    double t = (_height - ray.origin().z()) / ray.direction().z();
    if ( t >= 0.0 ) {
      Vector r( ray.origin().x() + ray.direction().x() * t,
		ray.origin().y() + ray.direction().y() * t,
		0 );
      if(r.length2() <= _radius*_radius) 
	hit.hit(t, this, matl);
    }
  }

  // Bottom
  if ( Abs(ray.direction().z()) > 1e-6 ) {
    double t = (-ray.origin().z()) / ray.direction().z();
    if ( t >= 0.0 ) {
      Vector r( ray.origin().x() + ray.direction().x() * t,
		ray.origin().y() + ray.direction().y() * t,
		0 );
      if(r.length2() <= _radius*_radius) 
	hit.hit(t, this, matl);
    }
  }
#endif

}

void Cylinder::getNormal(Vector& normal, RenderContext&context,
		      const Point& hitpos, const Ray&ray,
		      const HitRecord&hit) const
{
  // Hitpos is in transformed space. Get the position in regular
  // space. Then, get normal in regular space, and transform the
  // normal back to transformed space. Normalize the normal vector as
  // well.
  Point hitRegular = _mInverse * hitpos;
  Vector normalRegular;

  normalI( normalRegular, context, hitRegular, ray, hit );

  normal = _matrix.transform(normalRegular);
}

void Cylinder::normalI(Vector& normal, RenderContext&,
                   const Point& hit, const Ray&, const HitRecord&) const
{

  if (ABSCLOSE(hit.z(), _height, 1e-6 ) ) {
    normal = Vector(0,0,1);
    return;
  }
  if (ABSCLOSE(hit.z(), 0, 1e-6 ) ) {
    normal = Vector(0,0,-1);
    return;
  } 
  
  // Don't need to translate here...
  normal = _matrix.transform(Vector(hit.x(), hit.y(), 0));
  
  normal.normalize();
}
