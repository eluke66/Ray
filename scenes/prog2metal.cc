#include "all.h"
Scene* scene2metal()
{
  Scene* scene = new Scene();
  scene->setBackground(new ConstantBackground(Color(.1, .2, .3)));
  Group* world = new Group();
  world->addObject(new Sphere(new PhongMaterial(Color(1, .2, .2), .6, .4, Color(1,1,1), 20),
                              Point(-2, -0.5, 2), 1.2));
  world->addObject(new Sphere(new MetalMaterial(Color(.2, .8, .8), 50),
                              Point(1.5, 1, 4), 2));
  world->addObject(new Plane(new LambertianMaterial(Color(.8, .8, .8), .6, .4),
                             Vector(0,0,1), Point(0,0,-.1)));
  scene->setObject(world);

  scene->setAmbient(Color(.4, .4, .4));
  scene->addLight(new PointLight(Point(20, 30, 100), Color(.9,.9,.9)));
  scene->addLight(new PointLight(Point(-40, -30, 50), Color(.3,.1,.1)));

  scene->setCamera(new PinholeCamera(Point(1, -18, 3.5),
                                     Point(-1,1, 2),
                                     Vector(0, 0, 1),
                                     30));

   scene->setMaxRayDepth(25 );
 return scene;
}

ADD_SCENE(scene2metal)
  
