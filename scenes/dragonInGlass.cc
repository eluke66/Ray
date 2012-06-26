#include "all.h"
#include "IO/ObjectLoader.h"
#include "Core/Position.h"

Scene* dragonInGlass()
{
  Scene* scene = new Scene();
  scene->setBackground(new ConstantBackground(Color(0.5, 0.8, 0.9)));
  Group* world = new Group();
  Material* groundmatl = new LambertianMaterial(Color(0.95, 0.65, 0.35), .6, .4);
  world->addObject(new Plane(groundmatl,
                            Vector(0,0,1), Point(0,0,2.5)));

  Object * dragon = ObjectLoader::LoadObject( "Objects/dragon_vrip.ply", "" );

  Material* happymatl = new PhongMarbleMaterial(Color(0.0, 1, 0.0), // Color 1
                                             Color(1, 1, 1.0), // Color 2
                                             0.5, // scale
                                             8, // octaves
                                             20, // turbulence scale
                                             1, // frequency scale
                                             2, // lacunarity
                                             0.6, // gain
                                             0.6, 0.4, Color(1,1,1), 120);
  dragon->setMaterial( happymatl );
  Position p;
  p.translate( Vector(-2,-2,3.8), true );
  p.rotate( Vector(270,-45,0) );
  p.scale( 0.75, Position::SCALE_MAX );
  p.transform(dragon);

  world->addObject( dragon );
  double lenscale = 2.5;
  Color glassc(pow(0.93, lenscale), pow(0.80, lenscale), pow(0.87, lenscale));
  Material *glass = new DielectricMaterial(2.05, glassc, 100);
  Sphere * glassSphere = new Sphere( glass, Point( 0, -2, 2.8 ), 1.4 );
  world->addObject(glassSphere);
  scene->setObject(world);

  int samples = 400;
  scene->setAmbient(Color(.4, .4, .4));
  Sphere * lightSphere = new Sphere( NULL, Point(20, 30, 100), 5.0);
  scene->addLight( new AreaLight(*lightSphere, Color(.9,.9,.9), samples) );
  Sphere * ls = new Sphere(NULL, Point(-40, -30, 50), 5.0 );
  scene->addLight( new AreaLight( *ls, Color(.5,.3,.3), samples ));
  //scene->addLight(new PointLight(Point(-40, -30, 50), Color(.5,.3,.3)));
  
  scene->setCamera(new PinholeCamera(Point(8, -18, 8.5),
                                     Point(-4.7, 2.5, 2.5),
                                     Vector(0, 0, 1),
                                     15));
  scene->setMaxRayDepth(25 );
  scene->setMinAttenuation(.01);

  return scene;
}

ADD_SCENE(dragonInGlass);
