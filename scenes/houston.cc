#include "all.h"
#include "IO/ObjectLoader.h"
#include "Core/Position.h"

Scene* houston()
{
  Scene* scene = new Scene();
  scene->setBackground(new ConstantBackground(Color(0.0, 0.0, 0.0)));
  Group* world = new Group();
 
  Object * a1 = ObjectLoader::LoadObject( "Objects/HOUSTON.3DS", "" );

  Position p;
  p.translate( Vector(-2,-2,3.8), true );
  p.scale( 1, Position::SCALE_MAX );
  p.transform(a1);

  world->addObject( a1 );

  
  scene->setObject(world);

  //  scene->setAmbient(Color(.2, .2, .2));
  int nSamples = 10;
  Sphere * lightSphere = new Sphere( NULL, Point(20, 30, 100), 5.0);
  scene->addLight( new AreaLight(*lightSphere, Color(.9,.9,.9), nSamples) );
  Sphere * ls = new Sphere(NULL, Point(-40, -30, 50), 5.0 );
  scene->addLight( new AreaLight( *ls, Color(.3,.3,.3), nSamples ));
  
  scene->setCamera(new ThinLensCamera(Point(8, -18, 8.5),
				      Point(-4.7, 2.5, 2.5),
				      Vector(0, 0, 1),
				      15,
				      20, 8.0,
				      40 ));
  scene->setMaxRayDepth(10);
  scene->setMinAttenuation(.01);

  return scene;
}

ADD_SCENE(houston);
