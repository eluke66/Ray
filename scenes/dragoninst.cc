#include "all.h"
#include "Core/Position.h"
#include "Geometry/Instance.h"

Scene* dragoninst()
{
  Scene* scene = new Scene();
  scene->setBackground(new ConstantBackground(Color(0.5, 0.8, 0.9)));
  Group* world = new Group();
  
  Object * dragon =
    ObjectLoader::LoadObject( "Objects/dragon_vrip.ply", "" );
  dragon->setMaterial( new LambertianMaterial( Color(0,1,0), 0.6, 0.4 ) );
  world->addObject(dragon);
  
  for ( int i = 0; i < 5; ++i ) {
    Position p;
    Vector v = Vector::Rand() * 5;
    std::cerr << v << std::endl;
    p.translate( v, true );
    p.rotate( Vector( 270 * drand48(),
		      45 * drand48(),
		      11 * drand48() ) );
    p.scale( 3*drand48(), Position::SCALE_MAX );

    if ( getenv("NO_INSTANCE") ) {
      Object * d =
	ObjectLoader::LoadObject( "Objects/dragon_vrip.ply", "" );
      d->setMaterial(  new LambertianMaterial( Color(drand48(),
						     drand48(),
						     drand48()),
					       drand48(),
					       drand48() ) );
      p.transform(d);
      world->addObject(d);
    }
    else {
      Instance * d = new Instance( p.getMatrix(dragon),
				   dragon,
				   new LambertianMaterial( Color(drand48(),
								 drand48(),
								 drand48()),
							   drand48(),
							   drand48() ) );
      world->addObject(d);
    }
    
  }
      
  scene->setObject(world);
  
  scene->addLight(new PointLight(Point(20, -30, 30), Color(.9,.9,.9)));

  scene->setCamera(new PinholeCamera(Point(0, -20, 12.5),
				       Point(0, 0, 0),
				       Vector(0, 0, 1),
				       55));
  scene->setAmbient(Color(.2, .2, .2));
  scene->setMaxRayDepth(125 );
  scene->setMinAttenuation(.01);

  return scene;
}

ADD_SCENE(dragoninst);
