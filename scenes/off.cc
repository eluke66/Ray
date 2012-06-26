#include "all.h"
#include "IO/ObjectLoader.h"
#include "Core/Position.h"

Scene* off()
{
  Scene* scene = new Scene();
  scene->setBackground(new ConstantBackground(Color(0.5, 0.8, 0.9)));
  Group* world = new Group();
  Material* groundmatl = new LambertianMaterial(Color(0.95, 0.65, 0.35), .6, .4);
  world->addObject(new Plane(groundmatl,
                            Vector(0,0,1), Point(0,0,2.5)));

  Object * spongecube = ObjectLoader::LoadObject( "Objects/spongecube1.aoff", "" );
  Position p;
  p.translate( Vector(-6,4,3), true );
  p.scale( 0.5, Position::SCALE_MAX );
  p.transform(spongecube);

  world->addObject( spongecube );
  scene->setObject(world);

  scene->setAmbient(Color(.4, .4, .4));
  Sphere * lightSphere = new Sphere( NULL, Point(20, 30, 100), 5.0);
  scene->addLight( new AreaLight(*lightSphere, Color(.9,.9,.9), 10) );
  
  scene->setCamera(new PinholeCamera(Point(8, -18, 8.5),
                                     Point(-4.7, 2.5, 2.5),
                                     Vector(0, 0, 1),
                                     15));
  scene->setMaxRayDepth(25 );
  scene->setMinAttenuation(.01);
  scene->setSampling(Scene::Jittered, 4);

  return scene;
}

ADD_SCENE(off);
