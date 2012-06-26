#include "all.h"
#include "IO/ObjectLoader.h"
#include "Core/Position.h"

Scene* happyclear()
{
  Scene* scene = new Scene();
  scene->setBackground(new ConstantBackground(Color(0.5, 0.8, 0.9)));
  Group* world = new Group();
  Material *groundmatl = new PhongWoodMaterial(Color(227, 210, 187)/255.,
                                              Color(181, 160, 117)/255.,
					      30, // scale
					      2, // octaves
					      5, .04, // ring noise scale, frequency
					      1, .1, // center noise scale, frequency
					      2, // lacunarity
					      0.6, // gain
					      10, // expon
                                              0.5, 0.5, Color(0,0,0), 120);
  world->addObject(new Plane(groundmatl,
                            Vector(0,0,1), Point(0,0,2.5)));

  Object * happy = ObjectLoader::LoadObject( "Objects/happy_vrip_res2.ply", "" );

  Material * happymatl = new DielectricMaterial( 1.05, Color(1,1,1), 100 ); 

  happy->setMaterial( happymatl );
  Position p;
  p.translate( Vector(-2,-2,3.8), true );
  p.rotate( Vector(270,-45,0) );
  p.scale( 0.5, Position::SCALE_MAX );
  p.transform(happy);

  world->addObject( happy );
  scene->setObject(world);

  scene->setAmbient(Color(.2, .2, .2));
  Sphere * lightSphere = new Sphere( NULL, Point(20, 30, 100), 5.0);
  scene->addLight( new AreaLight(*lightSphere, Color(.9,.9,.9), 200) );
  Sphere * ls = new Sphere(NULL, Point(-40, -30, 50), 5.0 );
  scene->addLight( new AreaLight( *ls, Color(.5,.3,.3), 200 ));
  
  scene->setCamera(new PinholeCamera(Point(8, -18, 8.5),
                                     Point(-4.7, 2.5, 2.5),
                                     Vector(0, 0, 1),
                                     15));
  scene->setMaxRayDepth(125 );
  scene->setMinAttenuation(.01);
  scene->setSampling(Scene::Jittered, 2);

  return scene;
}

ADD_SCENE(happyclear);
