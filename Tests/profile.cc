#include <iostream>
#include "Core/Ray.h"
#include "Geometry/Water.h"
#include "Core/RenderContext.h"
#include "Core/HitRecord.h"
#include "Core/BoundingBox.h"
#include "Geometry/Sphere.h"
#include "Material/BasicShade.h"
#include "Geometry/Box.h"

void profile( Object * object, unsigned long numRuns ) {
  RenderContext context(NULL);
  HitRecord hit(DBL_MAX);

  BoundingBox b;
  object->getBounds(b);
  Sphere outside(NULL, b.getCenter(), b.getSize());
  Point origin;
  Ray ray;
  
  for (unsigned long i = 0; i < numRuns; ++i ) {
    outside.randomPoint(origin);
    ray.set( origin, Vector::Rand() );
    
    object->intersect( hit, context, ray );
  }
}

int
main(int argc, char ** argv) {
  
  Material *mat = new BasicShade(Color(1));
  //Object *obj = new Box(mat, Point(-1,-1,-1), Point(1,1,1));
  Object *obj = new Water(mat, 512, 1, 1, 265 * 3.1415 / 180, .00000775, 2.0,
			  Point(-10, -10, 0 ), Point(10,10,0));
  unsigned long numShots = 10000;
  
  if ( argc > 1 ) {
    numShots = atoll(argv[1]);
  }
  profile( obj, numShots );
}


typedef Scene *(*SceneGetter)(FILE *, const char *);
SceneGetter mainSceneGetter = NULL;
SceneGetter interactiveSceneGetter = NULL;
