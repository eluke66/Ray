#include "all.h"
Scene* highlight()
{
  Scene* scene = new Scene();
  scene->setBackground(new ConstantBackground(Color(.1, .2, .3)));
  Group* world = new Group();
  world->addObject(new Sphere(new DielectricMaterial(1.1, Color(1, .2, .2), 20),
                              Point(-2, -0.5, 2), 1.2));
  world->addObject(new Sphere(new PhongMaterial(Color(.2, 1, 1), .6, .4, Color(1,1,1), 50),
                              Point(1.5, 1, 4), 2));
  world->addObject(new Plane(new LambertianMaterial(Color(.8, .8, .8), .6, .4),
                             Vector(0,0,1), Point(0,0,-.1)));
  scene->setObject(world);

  scene->setAmbient(Color(.4, .4, .4));
  scene->addLight(new PointLight(Point(-5, 17, 0.5), Color(1, 1, 1)));

  scene->setCamera(new PinholeCamera(Point(1, -18, 3.5),
                                     Point(-1,-1, 2),
                                     Vector(0, 0, 1),
                                     30));

  scene->setMaxRayDepth(25);
  return scene;
}

ADD_SCENE(highlight)
  
