#include "all.h"
#include "IO/ObjectLoader.h"
#include "Core/Position.h"

Scene* t3ds()
{
  Scene* scene = new Scene();
  scene->setBackground(new ConstantBackground(Color(0.5, 0.8, 0.9)));
  Group* world = new Group();
  Material* groundmatl = new CheckerMaterial(Point(0,0,0), Vector(0.5,0.5,0),
					     Vector(-0.5,0.5,0),
					     new PhongMaterial(Color(1, 1, 1),
							       0.6, 0.4,
							       Color(1,1,1), 60),
					     new PhongMaterial(Color(0, 0, 0),
							       0.6, 0.4,
							       Color(1,1,1), 60));
  
  world->addObject(new Plane(groundmatl,
                            Vector(0,0,1), Point(0,0,2.5)));

  
  Object * lam = ObjectLoader::LoadObject( "Objects/PIANO.3DS", "" );
  Position p;
  p.translate( Vector(-2,-2,3), true );
  p.rotate( Vector(0,0,0) );
  p.scale( 1, Position::SCALE_MAX );
  p.transform(lam);

  world->addObject( lam );
  
  scene->setObject(world);

  //  scene->setAmbient(Color(.2, .2, .2));
  Sphere * lightSphere = new Sphere( NULL, Point(20, 30, 100), 5.0);
  scene->addLight( new AreaLight(*lightSphere, Color(.9,.9,.9), 20) );
  Sphere * ls = new Sphere(NULL, Point(-40, -30, 50), 5.0 );
  scene->addLight( new AreaLight( *ls, Color(.9,.9,.9), 20 ));
  
  scene->setCamera(new PinholeCamera(Point(8, -18, 8.5),
                                     Point(-4.7, 2.5, 2.5),
                                     Vector(0, 0, 1),
                                     15));
  scene->setMaxRayDepth(25 );
  scene->setMinAttenuation(.01);
  scene->setFilter(FilterFactory::findFilter2D("LanczosSinc3"));
  
  return scene;
}

ADD_SCENE(t3ds);
