#include "all.h"
#include "IO/ObjectLoader.h"
#include "Core/Position.h"

Scene* argonath()
{
  Scene* scene = new Scene();
  scene->setBackground(new ConstantBackground(Color(0.5, 0.8, 0.9)));
  Group* world = new Group();
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
  world->addObject(new Plane(ground,
                            Vector(0,0,1), Point(0,0,2.5)));

  Object * a1 = ObjectLoader::LoadObject( "Objects/Argonath.3ds", "" );

  Position p;
  p.translate( Vector(-2,-2,3.8), true );
  p.rotate( Vector(0,0,-60) );
  p.scale( 0.3, Position::SCALE_MAX );
  p.transform(a1);

  world->addObject( a1 );


  {
    Object * a1 = ObjectLoader::LoadObject( "Objects/Argonath.3ds", "" );
    
    //Material * m1 = new DielectricMaterial( 1.05, Color(1,1,1), 100 );
    
    //a1->setMaterial(m1);
    Position p;
    p.translate( Vector(-3,2,3.8), true );
    p.rotate( Vector(0,0,-60) );
    p.scale( 0.3, Position::SCALE_MAX );
    p.transform(a1);
    
    world->addObject( a1 );
  }
  
  scene->setObject(world);

  //  scene->setAmbient(Color(.2, .2, .2));
  int nSamples = 100;
  Sphere * lightSphere = new Sphere( NULL, Point(20, 30, 100), 5.0);
  scene->addLight( new AreaLight(*lightSphere, Color(.9,.9,.9), nSamples) );
  Sphere * ls = new Sphere(NULL, Point(-40, -30, 50), 5.0 );
  scene->addLight( new AreaLight( *ls, Color(.3,.3,.3), nSamples ));
  
  scene->setCamera(new PinholeCamera(Point(8, -18, 8.5),
                                     Point(-4.7, 2.5, 2.5),
                                     Vector(0, 0, 1),
                                     15));
  scene->setMaxRayDepth(10);
  scene->setMinAttenuation(.01);

  return scene;
}

ADD_SCENE(argonath);
