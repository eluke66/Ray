#include "Geometry/Box.h"
#include "Core/Ray.h"
#include "Core/HitRecord.h"
#include "Core/RenderContext.h"
#include "Core/Point.h"
#include "Core/BoundingBox.h"

int main() {

  BoundingBox bb(Point(5, -25, 0), Point(15, -15, 5));
  Box b(NULL, Point(5, -25, 0), Point(15, -15, 5) );

  Ray ray( Point(0, 60, 12.5), Vector(0.128868, -0.955262, -0.266211) );
  HitRecord hit;
  RenderContext context(NULL);
  
  b.intersect(hit, context, ray);
  std::cerr << "HIT1? " << hit.getPrimitive() << std::endl;


  Point eyept = Point(0, 60, 12.5) + 1000.0 * Vector(0.128868, -0.955262, -0.266211);
  ray = Ray( eyept, -Vector(0.128868, -0.955262, -0.266211) );
  b.intersect(hit, context, ray);
  std::cerr << "HIT2? " << hit.getPrimitive() << std::endl;


  Point p(0, 60, 12.5);
  for ( int i = 0; i < 100; i++ ) {
    p = p + Vector(0.128868, -0.955262, -0.266211);
    std::cerr << p << std::endl;
    if ( bb.inside(p) ) {
      std::cerr << "\tHITHITHIT!!!!\n" << std::endl;
    }
  }
}
