
#include "Geometry/ScaledBox.h"
#include "Core/BoundingBox.h"
#include "Core/HitRecord.h"
#include "Core/Math.h"
#include "Core/Point.h"
#include "Core/Ray.h"
#include "Core/Vector.h"
#include "Core/Matrix.h"
#include <iostream>
using namespace std;

ScaledBox::ScaledBox(Material* material, const Point& p1, const Point& p2, double scale)
  : Box(material,p1,p2), _scale(scale)
{
  ALLOC;
  DALLOC(Box);
  computeLengths();
}

ScaledBox::~ScaledBox()
{
}

void
ScaledBox::computeLengths() {

  // Len is the ratio of the width of the box to the depth of the
  // box. Thus, if the box is 2x as wide as it is long, then by
  // default, items on the wide side will be stretched twice as
  // long. This measure effectively alters the 'hRepeat' parameter on
  // a texture.
  
  Vector v = max - min;
  _len = v.x() / v.y();
}

void
ScaledBox::transform(const Matrix4 &matrix) {
  Box::transform(matrix);
  computeLengths();
}

void ScaledBox::computeUVW(Vector& uvw, const RenderContext& context,
                           const Ray& ray, const HitRecord& hit) const
{
  Point hitpos = ray.origin()+ray.direction()*hit.minT();
  
  if(Abs(hitpos.x()-min.x()) < 1.e-6) // YZ
    uvw = Vector( (hitpos.z() - min.z()) / (max.z() - min.z()),
		  _len * (hitpos.y() - min.y()) / (max.y() - min.y()),
		  0 );
  else if(Abs(hitpos.x()-max.x()) < 1.e-6) // YZ
    uvw = Vector( (hitpos.z() - min.z()) / (max.z() - min.z()),
		  _len * (hitpos.y() - min.y()) / (max.y() - min.y()),
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

  uvw *= _scale;
}

