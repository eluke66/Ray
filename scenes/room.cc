#include "all.h"
#include "Geometry/BoundedPlane.h"

const float ROOM_WIDTH  = 7;
const float ROOM_HEIGHT = 7;
const float ROOM_DEPTH  = 7;
const float THICKNESS   = 1.0f;
Scene* room()
{
  Scene* scene = new Scene();
  scene->setBackground(new ConstantBackground(Color(0.5, 0.8, 0.9)));
  Group* world = new Group();

  Material *wall = new LambertianMaterial(Color(0.95, 0.95, 0.95), .6, .4);



  Box *floor =
    new Box(wall,
	    Point( -ROOM_WIDTH, -ROOM_DEPTH, -ROOM_HEIGHT-THICKNESS ),
	    Point( ROOM_WIDTH,   ROOM_DEPTH, -ROOM_HEIGHT ) );
  world->addObject(floor);
  Box *ceil =
    new Box(wall,
	    Point( -ROOM_WIDTH, -ROOM_DEPTH, ROOM_HEIGHT ),
	    Point( ROOM_WIDTH, ROOM_DEPTH, ROOM_HEIGHT + THICKNESS ) );
  world->addObject(ceil);
  
  
  Box *back =
    new Box( wall,
	     Point( -ROOM_WIDTH, ROOM_DEPTH, -ROOM_HEIGHT-THICKNESS ),
	     Point(  ROOM_WIDTH, ROOM_DEPTH+THICKNESS, ROOM_HEIGHT+THICKNESS ) );
  world->addObject(back);

#ifdef BLOCKS_VIEW
  Box *front =
    new Box( wall,
	     Point( -ROOM_WIDTH, -ROOM_DEPTH, -ROOM_HEIGHT-THICKNESS ),
	     Point(  ROOM_WIDTH, -ROOM_DEPTH-THICKNESS, ROOM_HEIGHT+THICKNESS ) );
  world->addObject(front);
#endif

  Box *left =
    new Box(wall,
	    Point( -ROOM_WIDTH-THICKNESS, -ROOM_DEPTH-THICKNESS, -ROOM_HEIGHT-THICKNESS ),
	    Point( -ROOM_WIDTH,            ROOM_DEPTH+THICKNESS,  ROOM_HEIGHT+THICKNESS ) );
  world->addObject(left);
  
  Box *rbot =
    new Box( wall,
	     Point(ROOM_WIDTH,          -ROOM_DEPTH, -ROOM_HEIGHT-THICKNESS ),
	     Point(ROOM_WIDTH+THICKNESS, ROOM_DEPTH, -ROOM_HEIGHT+2.0*ROOM_HEIGHT/3.0 ) );
  world->addObject(rbot);

  Box *rtop =
    new Box( wall,
	     Point(ROOM_WIDTH,          -ROOM_DEPTH,  ROOM_HEIGHT-2.0*ROOM_HEIGHT/3.0 ),
	     Point(ROOM_WIDTH+THICKNESS, ROOM_DEPTH,  ROOM_HEIGHT+THICKNESS ) );
  world->addObject(rtop);

  Box *rfront = 
  new Box( wall,
	   Point(ROOM_WIDTH,          -ROOM_DEPTH,  -ROOM_HEIGHT-THICKNESS ),
	   Point(ROOM_WIDTH+THICKNESS,-ROOM_DEPTH + 2.0*ROOM_DEPTH/3.0,  ROOM_HEIGHT+THICKNESS ) );
  world->addObject(rfront);

  Box *rback = 
  new Box( wall,
	   Point(ROOM_WIDTH,          ROOM_DEPTH-2.0*ROOM_DEPTH/3.0,  -ROOM_HEIGHT-THICKNESS ),
	   Point(ROOM_WIDTH+THICKNESS,ROOM_DEPTH,  ROOM_HEIGHT+THICKNESS ) );
  world->addObject(rback);

  scene->setObject(world);

  //scene->setAmbient(Color(.4, .4, .4));
#if 0
  Sphere * lightSphere = new Sphere( NULL, Point(ROOM_WIDTH+4, 0, 0), 2.0);
  scene->addLight( new SphereLight(*lightSphere, Color(900), 100) );
#else

#if 0
  scene->addLight( new DirectionalLight( Vector(-1,0,0), Color(100.0) ) );
#else
  
  scene->addLight( new PlaneLight( *(new BoundedPlane( NULL,
						     Vector(-1,0,0),
						     Point( ROOM_WIDTH + THICKNESS + 1, 0, 0 ),
						       
						     Point( ROOM_WIDTH + THICKNESS,
							    -ROOM_DEPTH + 3.0*ROOM_DEPTH/4.0,
							    -ROOM_HEIGHT+ 3.0*ROOM_HEIGHT/4.0 + 2 ),
						       
						     Point( ROOM_WIDTH + THICKNESS + 1.1,
							    ROOM_DEPTH-3.0*ROOM_DEPTH/4.0,
							    ROOM_HEIGHT-3.0*ROOM_HEIGHT/4.0 + 2) )),
				   Color(100.0),
				   100 ) );
						     
							    
#endif
  
#endif
  
  scene->setCamera(new PinholeCamera(Point(0,-17,0),
                                     Point(0,0,0),
                                     Vector(0, 0, 1),
                                     75));
  scene->setMaxRayDepth(25 );
  scene->setMinAttenuation(.01);

  return scene;
}

ADD_SCENE(room);
