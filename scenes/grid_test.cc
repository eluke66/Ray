#include "all.h"
Scene* gridtest() {
  Scene* scene=new Scene();

  GriddedGroup* grid = new GriddedGroup(1);
  grid->addObject(new Sphere(new LambertianMaterial(Color(1, .2, .2), .6, .4),
                              Point(-2, -0.5, 2), 1.2));
#if 1
  grid->addObject(new Sphere(new LambertianMaterial(Color(.2, 1, 1), .6, .4),
                              Point(1.5, 1, 4), 2));
#endif
  scene->setObject(grid);

  scene->setAmbient(Color(.4, .4, .4));
  scene->addLight(new PointLight(Point(20, 30, 100), Color(.9,.9,.9)));
  scene->addLight(new PointLight(Point(-40, -30, 50), Color(.3,.1,.1)));

  scene->setCamera(new PinholeCamera(Point(1, -18, 3.5),
                                     Point(-1,1, 2),
                                     Vector(0, 0, 1),
                                     30));

  scene->setBackground(new ConstantBackground(Color(.1, .2, .3)));

  return scene;
}

ADD_SCENE(gridtest)
  
