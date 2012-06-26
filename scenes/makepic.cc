#include "all.h"
#include "IO/ObjectLoader.h"
#include "Core/Position.h"

Scene* makepic()
{
  Scene* scene = new Scene();
  scene->setBackground(new ConstantBackground(Color(0.5, 0.8, 0.9)));
  Group* world = new Group();

  char * objPath = getenv( "OBJECT_TO_RENDER" );
  if ( !objPath )
    assert(!"No object to render!");
  
  Object * obj = ObjectLoader::LoadObject( objPath, "" );
  obj->setMaterial( new LambertianMaterial(Color(1,1,1), 0.6, 0.4),
		    false );

  if(0){
    Position p1;
    p1.rotate( Vector(90, 0, 0) );
    p1.transform(obj);
  }
  
  Position p;
  double scaleFactor = Position::ScaleToFit( obj, BoundingBox( Point( -2, 0, -2.5 ), Point( 2, 4, 1.5) ) );
  p.translate( Vector(0,0,0), true );
  p.scale( scaleFactor, Position::SCALE_REL );
  p.transform(obj);

  world->addObject( obj );
  scene->setObject(world);

  scene->setAmbient(Color(.4,.4,.4));
  scene->addLight( new PointLight( Point(-3, -3.5, 4.5 ), Color( .2,.2,.2 ) ) );
  scene->addLight( new PointLight( Point(0, -6.6, 7.5 ), Color( .2,.2,.2 ) ) );
  scene->addLight( new PointLight( Point(4, -18, 5 ), Color( .2,.2,.2 ) ) );
  scene->setCamera(new PinholeCamera(Point(4, -18, 5),
                                     Point(0, 0, 0),
                                     Vector(0, 0, 1),
                                     15));
  scene->setMaxRayDepth(125 );
  scene->setMinAttenuation(.01);

  return scene;
}

ADD_SCENE(makepic);
