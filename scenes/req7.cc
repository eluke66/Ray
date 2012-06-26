#include "all.h"
Scene* scene7()
{
  Scene* scene = new Scene();
  scene->setBackground(new ConstantBackground(Color(.0, .2, .1)));
  Group* world = new Group();

  // Material* image = new PhongImageMaterial("textures/land_ocean_ice_cloud_8192.ppm", 0.6, 0.4, Color(1,1,1), 80);
  Material* image = new PhongImageMaterial("textures/land_ocean_ice_cloud_1024.ppm", 0.6, 0.4, Color(1,1,1), 80);
  world->addObject(new SpherePolar(image,
                                   Point(-2, -0.5, 2), 1.6,
                                   Vector(0,-.4,1), Vector(-1,-.2,0)));
  // Material* nightimage = new PhongImageMaterial("textures/land_lights_16384.ppm", 0.6, 0.4, Color(0,0,0), 80);
  Material* nightimage = new PhongImageMaterial("textures/land_lights_1024.ppm", 0.6, 0.4, Color(0,0,0), 80);
  world->addObject(new SpherePolar(nightimage,
                                   Point(0, 2.0, 3.5), 1.6,
                                   Vector(0,-.5,1), Vector(.5,1,0)));

  Material* marble = new PhongMarbleMaterial(Color(0.1, 0.2, 0.5), // Color 1
                                             Color(0.7, 0.8, 1.0), // Color 2
                                             0.5, // scale
                                             8, // octaves
                                             20, // turbulence scale
                                             1, // frequency scale
                                             2, // lacunarity
                                             0.6, // gain
                                             0.6, 0.4, Color(1,1,1), 120);
  world->addObject(new Sphere(marble,
                              Point(1.5, 1, 1.8), 1.6));

  Material* ground = new CheckerMaterial(Point(0,0,0), Vector(0.5,0.5,0), Vector(-0.5,0.5,0),
                                         new MetalMaterial(Color(0.8, 0.8, 0.8), 100),
                                         new PhongMaterial(Color(0.5, 0.5, 0.5), 0.6, 0.4,
                                                           Color(1,1,1), 60));
  world->addObject(new Triangle(ground,
                                Point(-30,-30,-.1), Point(30,-30,-.1), Point(30,30,-.1)));
  world->addObject(new Triangle(ground,
                                Point(-30,-30,-.1), Point(30,30,-.1), Point(-30,30,-.1)));
  scene->setObject(world);

  scene->setAmbient(Color(.4, .4, .4));
  scene->addLight(new PointLight(Point(100, -80, 100), Color(1,1,1)));

  scene->setCamera(new PinholeCamera(Point(1, -15, 4.0),
                                     Point(-0.5,.75, 2),
                                     Vector(0, 0, 1),
                                     25));
  scene->setMaxRayDepth(10);
  scene->setMinAttenuation(.01);
  scene->setSampling(Scene::Jittered, 5);
  
  return scene;
}

ADD_SCENE(scene7)
