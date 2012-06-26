#include "all.h"
#include "Background/StarfieldBackground.h"
#include "Core/Position.h"
#include "Geometry/Instance.h"
Scene* waves()
{
  Scene* scene = new Scene();
  //scene->setBackground(new ConstantBackground(Color(.1, .3, .3)));
  //scene->setBackground(new ConstantBackground(Color(0,0,0)));
  scene->setBackground(new StarfieldBackground(Color(0,0,0)));
  Group* world = new Group();


  Material *hfmatl = new PhongImageMaterial( "textures/watergrey.ppm",
					     0.6, 0.4,
					     Color(0,0,1),
					     100 );
  int nTiles = 2;
  Point p( -64.0, -64.0, 2 );
  double tileSize = 128.0 / (double)nTiles;

  Heightfield * h = new Heightfield(hfmatl,
				    "waves2k.hf",
				    p,
				    p+Vector(tileSize,tileSize,0) );
  float fudge = 0.037;
  for ( int x = 0; x < nTiles; ++x ) {
    for ( int y = 0; y < nTiles; ++y ) {
      Position pos;
      pos.translate( Vector(x*(tileSize-fudge), y*(tileSize-fudge), 0.), false );

      Instance *i = new Instance( pos.getMatrix( h ),
				  h,
				  hfmatl );
      world->addObject(i);
    }
  }
  
  Material *m =
    new _Material<TextureColor>( TextureColor( "textures/moonbright.jpg") );
  world->addObject( new SpherePolar( m, Point( -5, 85, 10 ), 3.9,
				     Vector(0,0,1), Vector(0,1,0)) );
  
  
  scene->setObject(world);

  scene->addLight(new PointLight(Point( -5, 70, 20 ), Color(.9,.9,.9) ) );

  scene->setCamera(new PinholeCamera(Point(0, -40, 20),
                                     Point(0, 0, 0),
                                     Vector(0, 0, 1),
                                     50));

  scene->setMaxRayDepth(10 );
  scene->setMinAttenuation(.01);
  
  return scene;
}

ADD_SCENE(waves)
