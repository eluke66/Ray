#include "all.h"
#include "Background/GradientBackground.h"
#include "Core/Position.h"
#include "Geometry/Instance.h"
#include "Geometry/Water.h"
#include <sstream>


Scene * water()
{
  Scene* scene = new Scene();
  Group* world = new Group();
  scene->setBackground( new GradientBackground( Color (.745, 0.462, 0.313 ),
						Color (0,0,1),
						false, 1.0 ) );

  int nTilesY = 4;
  int nTilesX = 2;
  Point p( -64.0, -20.0, 0 );
  double tileSizeX = 128.0 / (double)nTilesX;
  double tileSizeY = 128.0 / (double)nTilesY;

  Material *hfmatl = new LambertianMaterial( Color(0,0,1),
					     0.6, 0.4 );
  Water *water = new Water( hfmatl,
			    512,
			    true,
			    false,
			    265 * 3.1415 / 180,
			    .00000775f,
			    19.0f,
			    p,
			    p + Vector(tileSizeX,tileSizeY,0) );

  double fudge = 0.01;
  float waterMin = water->getMinimum();
  
  for ( int x = 0; x < nTilesX; ++x ) {
    for ( int y = 0; y < nTilesY; ++y ) {
      Position pos;
      pos.translate( Vector(x*(tileSizeX-fudge), y*(tileSizeY-fudge), -waterMin ), false );
      Instance *i = new Instance( pos.getMatrix( water ),
				  water,
				  hfmatl );
      world->addObject(i);
    }
  }

  scene->setObject( world );
  scene->setAmbient(Color(.2, .2, .2));

  scene->addLight(new PointLight(Point(20, -30, 30), Color(.9,.9,.9)));
  if ( getenv( "MOVE_CAMERA") ) {
    scene->setCamera(new PinholeCamera(Point(0 + 40*(drand48()-0.5), -40, 7.5 + 5*drand48()),
				       Point(0, 0, 0),
				       Vector(0, 0, 1),
				       55));
  }
  else {
    scene->setCamera(new PinholeCamera(Point(0, -60, 22.5),
				       Point(0, 0, 0),
				       Vector(0, 0, 1),
				       55));
  }
  scene->setMaxRayDepth(10);
  scene->setMinAttenuation(.01);
  
  return scene;

}


ADD_SCENE(water);
