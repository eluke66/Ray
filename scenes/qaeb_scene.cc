#include "all.h"

class sinfunction {
 public:
  double operator()(double x, double y) const
  {
    double r2 = (x*x+y*y)*4;
    return (sin(r2)/r2+1)*0.5;
  }
  void center( const Point&) {}
};

Scene* sceneqaeb()
{
  Scene* scene = new Scene();
  scene->setBackground(new ConstantBackground(Color(.1, .2, .3)));
  Group* world = new Group();
  Material* sinmatl = new LambertianMaterial(Color(1, 1, 0), .8, .2);
  int xres = 1600;
  double fov = 20;
  double epsilon = 0.5;
  double e = epsilon*tan(fov*M_PI/180/2)/xres;
  world->addObject(new QAEB<sinfunction>(sinmatl, Point(-4,-4,2),
                                         Point(4,4,4), e));
  scene->setObject(world);

  scene->setAmbient(Color(.4, .4, .4));
  scene->addLight(new PointLight(Point(80, 60, 100), Color(.9,.9,.9)));
  scene->addLight(new PointLight(Point(-40, -30, 50), Color(.3,.1,.1)));

  scene->setCamera(new PinholeCamera(Point(10, -18, 13.5),
                                     Point(0,0,3),
                                     Vector(0, 0, 1),
                                     fov));

  return scene;
}

ADD_SCENE(sceneqaeb)
  
