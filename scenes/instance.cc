#include "all.h"
#include "Core/Position.h"
#include "Geometry/Instance.h"

Scene* instance()
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
			     Vector(0,0,1), Point(0,0,-2.5)));

  Box *basic_box = new Box( new LambertianMaterial( Color(0,1,0), 0.6, 0.4 ),
			    Point( -1, -1, -1 ),
			    Point( 1, 1, 1 ) );
			   
  world->addObject( basic_box );

  {
    Position p;
    p.translate( Vector(-3,0,0), false );
    
    Instance * left_box = new Instance( p.getMatrix(basic_box),
					basic_box );
    world->addObject( left_box );
  }

  {
    Position p;
    p.rotate( Vector(45, 0, 0) );
    p.translate( Vector( 3, 0, 0 ), false );
    Instance * right_box = new Instance( p.getMatrix(basic_box),
					 basic_box );
    world->addObject( right_box );
  }

  {
    Position p;
    p.rotate( Vector(15, 7, 61) );
    p.translate( Vector( 0, 5, 3 ), false );
    p.scale( 3.0, Position::SCALE_REL );
    Instance * back_box = new Instance( p.getMatrix(basic_box),
					 basic_box );
    world->addObject( back_box );
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

ADD_SCENE(instance);
