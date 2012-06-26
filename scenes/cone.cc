#include "all.h"
#include "IO/ObjectLoader.h"
#include "Core/Position.h"


Scene* cone()
{
  Scene* scene = new Scene();
  scene->setBackground(new ConstantBackground(Color(.1, .3, .3)));
  Group* world = new Group();
  Object * cone = ObjectLoader::LoadObject( "Objects/cone.aoff", "" );
  {
    Position p;
    p.translate( Vector(-1, 3, 3.8), true );
    //    p.scale( 0.5, Position::SCALE_MAX );
    p.transform(cone);
  }
  cone->setMaterial( new PhongMaterial(Color(1, 1, 1),
				       0.6, 0.4,
				       Color(1,1,1), 60) );
  scene->setObject(world);

  scene->setAmbient(Color(.4, .4, .4));
  scene->addLight(new PointLight(Point(-40, -40, 100), Color(.9,.9,.9)));
  scene->addLight(new PointLight(Point(20, -40, 100), Color(.3,.1,.1)));

  scene->setCamera(new PinholeCamera(Point(8, -18, 8.5),
                                     Point(-4.7, 2.5, 2.5),
                                     Vector(0, 0, 1),
                                     15));

  return scene;
}

ADD_SCENE(cone)
