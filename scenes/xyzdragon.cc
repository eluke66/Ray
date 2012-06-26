#include "all.h"
#include "IO/ObjectLoader.h"
#include "Core/Position.h"

Scene* xyzdragon()
{
  Scene* scene = new Scene();
  scene->setBackground(new ConstantBackground(Color(0.5, 0.8, 0.9)));
  Group* world = new Group();
  Material* groundmatl = new LambertianMaterial(Color(0.95, 0.65, 0.35), .6, .4);
  world->addObject(new Plane(groundmatl,
                            Vector(0,0,1), Point(0,0,2.5)));

  Object * happy = ObjectLoader::LoadObject( "Objects/xyzrgb_dragon.ply", "" );

  Material* happymatl = new PhongMaterial(Color(227, 210, 187)/255.,
					  0.5, 0.5, Color(0,0,0), 120);

  happy->setMaterial( happymatl );
  Position p;
  p.translate( Vector(-2,-2,3.8), true );
  p.rotate( Vector(270,135,0) );
  p.scale( 1.0, Position::SCALE_MAX );
  p.transform(happy);

  world->addObject( happy );
  scene->setObject(world);

  scene->setAmbient(Color(.2, .2, .2));

  scene->addLight(new PointLight(Point(20, 30, 100), Color(.9,.9,.9)));
  scene->addLight(new PointLight(Point(-40, -30, 50), Color(.5,.3,.3)));
  
  scene->setCamera(new PinholeCamera(Point(8, -18, 8.5),
                                     Point(-4.7, 2.5, 2.5),
                                     Vector(0, 0, 1),
                                     15));
  scene->setMaxRayDepth(25 );
  scene->setMinAttenuation(.01);
  return scene;
}

ADD_SCENE(xyzdragon);
