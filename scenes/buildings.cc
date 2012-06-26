#include "all.h"
#include "IO/ObjectLoader.h"
#include "Core/Position.h"

Scene* buildings()
{
  Scene* scene = new Scene();
  scene->setBackground(new ConstantBackground(Color(0.2, 0.5, 1)));
  Group* world = new Group();
  Material* groundmatl = new CheckerMaterial(Point(0,0,0), Vector(1,0,0),
					     Vector(0,1,0),
					     new PhongMaterial(Color(1, 1, 1),
							       0.6, 0.4,
							       Color(1,1,1), 60),
					     new PhongMaterial(Color(0, 0, 0),
							       0.6, 0.4,
							       Color(1,1,1), 60));

  world->addObject(new Plane(groundmatl,
                            Vector(0,0,1), Point(0,0,0)));


  if (1){
    Object * lam = ObjectLoader::LoadObject( "Objects/office_sp_fournel.3ds", "" );
    Position p;
    p.translate( Vector(3,0,3.5), true );
    p.rotate( Vector(0,0,70) );
    p.scale( 5, Position::SCALE_MAX );
    p.transform(lam);
    world->addObject( lam );
  }

  if (1){
    Object *sky2 = ObjectLoader::LoadObject( "Objects/skyscraper2_mrealms.3ds", "" );
    Position p;
    p.translate( Vector(0,30,17), true );
    p.rotate( Vector(0,0,0) );
    p.scale( 6, Position::SCALE_MAX );
    p.transform(sky2);
    world->addObject(sky2);
  }

  int startY = 40;
  for ( int i = 0; i < 20; ++i ) 
  {
    Object *oak = ObjectLoader::LoadObject( "Objects/oak_mrealms.3ds", "" );
    Position p;
    p.translate( Vector(-10 + drand48(),startY-(i*5),4), true );
    p.rotate( Vector(0,0,0) );
    p.scale( 1, Position::SCALE_MAX );
    p.transform(oak);
    world->addObject(oak);
  }

  int numCols = 10;
  double colInc = M_PI / numCols;
  double radius = 1.5;
  Vector colCenter( 2, -27, 1.5 );
  for ( int i = 0; i < numCols; ++i )
    {
      Object * col = ObjectLoader::LoadObject( "Objects/Doriccolumn.3DS", "" );
      Position p;
      colCenter += Vector( radius*cos(i*colInc), radius*sin(i*colInc), 0);
      p.translate( colCenter, true );
      p.scale( 0.003, Position::SCALE_REL );
      p.transform(col);
      world->addObject(col);
    }
  scene->setObject(world);

  scene->setAmbient(Color(.2, .2, .2));
#if 0
  scene->addLight(new PointLight(Point(20, -30, 30), Color(.9,.9,.9)));
#else
  scene->addLight(new AreaLight( *new Sphere(NULL,Point(20,-30,30), 5.0),
				 Color(.9,.9,.9),
				 30 ) );
#endif
  scene->setCamera(new PinholeCamera(Point(0, -40, 6.5),
                                     Point(0, 0, 0),
                                     Vector(0, 0, 1),
                                     55));
				     
  scene->setMaxRayDepth(25 );
  scene->setMinAttenuation(.01);
  scene->setFilter(FilterFactory::findFilter2D("LanczosSinc3"));
  
  return scene;
}

ADD_SCENE(buildings);
