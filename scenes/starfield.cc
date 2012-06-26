#include "all.h"
#include "Background/StarfieldBackground.h"

Scene* starfield()
{
  Scene* scene = new Scene();
  
  scene->setBackground(new StarfieldBackground(Color(0,0,0)));
  Group* world = new Group();


  
  scene->setObject(world);

  scene->setCamera(new PinholeCamera(Point(0, -40, 20),
                                     Point(0, 0, 0),
                                     Vector(0, 0, 1),
                                     50));

  scene->setMaxRayDepth(10 );
  scene->setMinAttenuation(.01);
  
  return scene;
}

ADD_SCENE(starfield)
