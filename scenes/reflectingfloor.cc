#include "all.h"
#include "IO/ObjectLoader.h"
#include "Core/Position.h"

typedef _Material<WoodColor,PhongLighting,ReflectingDielectric> ReflectingWoodMaterial;

Scene* reflectingfloor()
{
  Scene* scene = new Scene();
  scene->setBackground(new ConstantBackground(Color(0.5, 0.8, 0.9)));
  Group* world = new Group();

  Material * groundmatl = new ReflectingWoodMaterial( WoodColor( Color(227, 210, 187)/255.,
								 Color(181, 160, 117)/255.,
								 30, // scale
								 2, // octaves
								 5, .04, // ring noise scale, frequency
								 1, .1, // center noise scale, frequency
								 2, // lacunarity
								 0.6, // gain
								 10 ),
						      PhongLighting( 0.5, 0.5, Color(0,0,0), 120),
						      ReflectingDielectric(0.3) );
  world->addObject(new Plane(groundmatl,
                            Vector(0,0,1), Point(0,0,2.5)));
  world->addObject( new Plane( new PhongMaterial(Color(1,0,0), 0.6, 0.4, Color(1,1,1), 90),
  			       Vector(0.3,0.7,0), Point(0,5,0) ) );
  world->addObject( new Plane( new PhongMaterial(Color(0,1,0), 0.6, 0.4, Color(1,1,1), 90),
  			       Vector(0.7,0.3,0), Point(-8,3,0) ) );

  Object * chair = ObjectLoader::LoadObject( "Objects/CHAIR4.3DS", "");
  Position p;
  p.translate( Vector(-2,-2,3.5), true );
  p.scale( 2, Position::SCALE_MAX );
  p.transform(chair);

  
  world->addObject(chair);
  
  scene->setObject(world);

  scene->setAmbient(Color(.2, .2, .2));
   scene->addLight( new PointLight( Point(-3, -3.5, 4.5 ), Color( .8,.8,.8 ) ) );
  
  scene->setCamera(new PinholeCamera(Point(8, -18, 8.5),
                                     Point(-4.7, 2.5, 2.5),
                                     Vector(0, 0, 1),
                                     15));
  scene->setMaxRayDepth(125 );
  scene->setMinAttenuation(.01);

  return scene;
}

ADD_SCENE(reflectingfloor);
