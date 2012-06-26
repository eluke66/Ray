#include "all.h"
Scene* sceneprog8()
{
  Scene* scene = new Scene();
  scene->setBackground(new ConstantBackground(Color(112/255., 204/255.,
                                                    255./255.)));
  Group* world = new Group();

  Material* volume = new PhongVolumeMaterial("data/CThead.hdr",
                                             "data/CThead.cmap",
                                             Point(1,0,1), Point(0,1,0), 0.5,
                                             0.999, 0.8, 0.2, Color(1,1,1), 100);
  world->addObject(new Box(volume, Point(0,0,0), Point(.75,1,1)));

  Material* marble = new PhongMarbleMaterial(Color(1, 1, 1), // Color 1
                                             Color(1, 0.2, 0.2), // Color 2
                                             0.5, // scale
                                             8, // octaves
                                             20, // turbulence scale
                                             1, // frequency scale
                                             2, // lacunarity
                                             0.6, // gain
                                             0.6, 0.4, Color(1,1,1), 120);

  Material* ground = new CheckerMaterial(Point(0,0,0), Vector(0.5,0.5,0),
                                         Vector(-0.5,0.5,0),
                                         marble,
                                         new PhongMaterial(Color(0, 0.25, 0),
                                                           0.6, 0.4,
                                                           Color(1,1,1), 60));
  world->addObject(new Triangle(ground,
                                Point(-20,-20,-.1), Point(20,-20,-.1), Point(20,20,-.1)));
  world->addObject(new Triangle(ground,
                                Point(-20,-20,-.1), Point(20,20,-.1), Point(-20,20,-.1)));

  Material *m = new MetalMaterial(Color(0.8, 0.8, 0.8), 100);
  world->addObject(new Sphere(m, Point(0.25, 2, 0.5), 0.35));
  scene->setObject(world);

  scene->setAmbient(Color(.4, .4, .4));
  scene->addLight(new PointLight(Point(-100, -80, 100), Color(1,1,1)));

  scene->setCamera(new PinholeCamera(Point(-3, -8, 3.0),
                                     Point(0, 0, 0.6),
                                     Vector(0, 0, 1),
                                     10));
  scene->setMaxRayDepth(10);
  scene->setMinAttenuation(.01);
  scene->setSampling(Scene::Jittered, 3);

  return scene;
}

ADD_SCENE(sceneprog8)
