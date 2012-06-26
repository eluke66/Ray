
#include "Geometry/Box.h"
#include "Core/BoundingBox.h"
#include "Core/HitRecord.h"
#include "Core/Math.h"
#include "Core/Point.h"
#include "Core/Ray.h"
#include "Core/Vector.h"
#include "Core/Matrix.h"
#include <iostream>
using namespace std;

Box::Box(Material* material, const Point& p1, const Point& p2)
  : Primitive(material)
{
  ALLOC;
  min = Min(p1, p2);
  max = Max(p1, p2);
}

Box::~Box()
{
}

void Box::transform(const Matrix4 &matrix) {
  min = matrix * min;
  max = matrix * max;
}

void Box::getBounds(BoundingBox& bbox) const
{
  bbox.extend(min);
  bbox.extend(max);
}

void Box::intersect(HitRecord& hit, const RenderContext&, const Ray& ray) const
{

  Vector idir = ray.inverseDirection();
  Vector v1 = (min-ray.origin())*idir;
  Vector v2 = (max-ray.origin())*idir;
  Vector vmin = Min(v1, v2);
  Vector vmax = Max(v1, v2);
  double near = vmin.maxComponent();
  double far = vmax.minComponent();
  //cerr << this << " min=" << min << ", max=" << max << '\n';
  //cerr << this << " v1=" << v1 << ", v2=" << v2 << '\n';
  //cerr << this << " orig=" << ray.origin() << ", dir=" << ray.direction() << '\n';
  //cerr << this << " vmin=" << vmin << ", near=" << near << ", vmax" << vmax << ", far=" << far << '\n';
  if(near < far){
    if(!hit.hit(near, this, matl)){
      hit.hit(far, this, matl);
    }
  }
}

void Box::computeUVW(Vector& uvw, const RenderContext& context,
                           const Ray& ray, const HitRecord& hit) const
{
  Point hitpos = ray.origin()+ray.direction()*hit.minT();
  
  if(Abs(hitpos.x()-min.x()) < 1.e-6) // YZ
    uvw = Vector( (hitpos.z() - min.z()) / (max.z() - min.z()),
		  (hitpos.y() - min.y()) / (max.y() - min.y()),
		  0 );
  else if(Abs(hitpos.x()-max.x()) < 1.e-6) // YZ
    uvw = Vector( (hitpos.y() - min.y()) / (max.y() - min.y()),
		  (hitpos.z() - min.z()) / (max.z() - min.z()),
		  0 );
  else if(Abs(hitpos.y()-min.y()) < 1.e-6) // XZ
    uvw = Vector( (hitpos.x() - min.x()) / (max.x() - min.x()),
		  (hitpos.z() - min.z()) / (max.z() - min.z()),
		  0 );
  else if(Abs(hitpos.y()-max.y()) < 1.e-6)
    uvw = Vector( (hitpos.x() - min.x()) / (max.x() - min.x()),
		  (hitpos.z() - min.z()) / (max.z() - min.z()),
		  0 );
  else if(Abs(hitpos.z()-min.z()) < 1.e-6) // XY
     uvw = Vector( (hitpos.x() - min.x()) / (max.x() - min.x()),
		   (hitpos.y() - min.y()) / (max.y() - min.y()),
		  0 );
  else
    uvw = Vector( (hitpos.x() - min.x()) / (max.x() - min.x()),
		  (hitpos.y() - min.y()) / (max.y() - min.y()),
		  0 );
}

void Box::getNormal(Vector& normal, RenderContext&, const Point& hitpos,
                    const Ray& ray, const HitRecord& hit) const
{
  if(Abs(hitpos.x()-min.x()) < 1.e-6)
    normal = Vector(-1,0,0);
  else if(Abs(hitpos.x()-max.x()) < 1.e-6)
    normal = Vector(1,0,0);
  else if(Abs(hitpos.y()-min.y()) < 1.e-6)
    normal = Vector(0,-1,0);
  else if(Abs(hitpos.y()-max.y()) < 1.e-6)
    normal = Vector(0,1,0);
  else if(Abs(hitpos.z()-min.z()) < 1.e-6)
    normal = Vector(0,0,-1);
  else if(Abs(hitpos.z()-max.z()) < 1.e-6)
    normal = Vector(0,0,1);
  else {
    std::cerr << "Hit: " << hitpos << ", min " << min << ", max " << max << std::endl;
    assert(0);
  }
}
