#include "all.h"
Scene* scene3()
{
  Scene* scene = new Scene();
  scene->setBackground(new ConstantBackground(Color(.1, .3, .3)));
  Group* world = new Group();
  Material* box1matl = new LambertianMaterial(Color(.8, .8, .8), .6, .4);
  world->addObject(new Box(box1matl,
                           Point(-4.5, -4.5, 0), Point(4.5, 4.5, 2)));
  Material* disk1matl = new LambertianMaterial(Color(1, .2, .2), .6, .4);
  world->addObject(new Disk(disk1matl,
                            Point(3., -3.3, 2.5), Vector(-1, -1, 1), .5));
  Material* disk2matl = new LambertianMaterial(Color(.2, 1, .2), .6, .4);
  world->addObject(new Disk(disk2matl,
                            Point(2.5, -3.3, 2.5), Vector(-1, -1, 1), .5));
  world->addObject(new Disk(disk1matl,
                            Point(2., -3.3, 2.5), Vector(-1, -1, 1), .5));
  world->addObject(new Disk(disk2matl,
                            Point(1.5, -3.3, 2.5), Vector(-1, -1, 1), .5));
  world->addObject(new Disk(disk1matl,
                            Point(1., -3.3, 2.5), Vector(-1, -1, 1), .5));
  Material* spherematl = new LambertianMaterial(Color(1, 0, 0), .6, .4);
  world->addObject(new Sphere(spherematl,
                              Point(-2, 2, 5), .8));
  Material* ringmatl = new LambertianMaterial(Color(.25, .25, 1), .6, .4);
  world->addObject(new Ring(ringmatl,
                            Point(-2, 2, 5), Vector(1, -1, 1), 1.2, 1.8));
  world->addObject(new Ring(ringmatl,
                            Point(-2, 2, 5), Vector(1, -1, 1), 2.2, 2.8));
  Material* trimatl = new LambertianMaterial(Color(1, 1, 0), .6, .4);
  world->addObject(new Triangle(trimatl,
                                Point(1,1,3), Point(3,-.5,2), Point(3, 1.5, 5)));
  Material* hfmatl = new LambertianMaterial(Color(.5, 1, 1), .8, .2);
  world->addObject(new Heightfield(hfmatl,
                                   "sinc_200_200.hf",
                                   Point(-3.5, -3.5, 2), Point(0, 0, 4)));
  Material* box2matl = new LambertianMaterial(Color(1, .4, 1), .6, .4);
  world->addObject(new Box(box2matl,
                           Point(-3.5, -3.5, 2), Point(0, 0, 2.75)));
  scene->setObject(world);

  scene->setAmbient(Color(.4, .4, .4));
  scene->addLight(new PointLight(Point(-40, -40, 100), Color(.9,.9,.9)));
  scene->addLight(new PointLight(Point(20, -40, 100), Color(.3,.1,.1)));

  scene->setCamera(new PinholeCamera(Point(-2.5, -20, 10),
                                     Point(0.5, 0, 3),
                                     Vector(0, 0, 1),
                                     30));

  return scene;
}

ADD_SCENE(scene3)
