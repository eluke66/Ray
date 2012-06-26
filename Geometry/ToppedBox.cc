
#include "Geometry/ToppedBox.h"
#include "Core/BoundingBox.h"
#include "Core/HitRecord.h"
#include "Core/Math.h"
#include "Core/Point.h"
#include "Core/Ray.h"
#include "Core/Vector.h"
#include "Core/Matrix.h"
#include <iostream>
using namespace std;

ToppedBox::ToppedBox(Material* boxMaterial, Material *topMaterial, const Point& p1, const Point& p2)
  : Box(boxMaterial,p1,p2), _topMaterial(topMaterial)
{
  ALLOC;
  DALLOC(Box);
}

ToppedBox::~ToppedBox()
{
}

void
ToppedBox::intersect(HitRecord& hit, const RenderContext& context, const Ray& ray) const
{
  double minT = hit.minT();
  Box::intersect(hit,context,ray);
  if ( minT == hit.minT() ) // Didn't hit it
    return;
  
  double z = ray.origin().z() + ray.direction().z()*hit.minT();
  if(Abs(z-max.z()) < 1.e-6)
    hit.setMaterial(_topMaterial);
}
