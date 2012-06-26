from Core import *
from Background import *
from Materials import *
from Geometry import *
from IO import *
from Filter import *

def all(world):

  ## BASIC #################
  #mat = lambertianMaterial( Color(1,0,0), 0.6, 0.4 )
  mat = schlickMaterial( Color(1,0,0), 0.3, 0.1, 0.1 )
  cyl = Cylinder( mat,
                  2.0, 1.0 )
  world.addObject(cyl)

  ##########################
  
  ## XLATED#################
  cyl1 = Cylinder( lambertianMaterial( Color(0,1,0), 0.6, 0.4 ),
                   1.0, 1.0 )
  p = Position()
  p.translate( Vector( -5, 1, 3 ), True )
  p.transform(cyl1)
  world.addObject(cyl1)
##########################
  
## ROTATE AND SCALE ######
  cyl2 = Cylinder( lambertianMaterial( Color(0,0,1), 0.6, 0.4 ),
                   2.0, 2.0 )
  p = Position()
  p.translate( Vector( 5, 1, -3 ), True )
  p.rotate( Vector(80,-45,0) );
  #p.scale( 2, Position.SCALE_REL );
  p.transform(cyl2)
  world.addObject(cyl2)
##########################
  
scene = Scene()
world = Group();
scene.setBackground( ConstantBackground( Color (.745, 0.462, 0.313 ) ) )

all(world)
#world.addObject(Cylinder( lambertianMaterial( Color(1,0,0), 0.6, 0.4 ), 20.0, 5.0 ))



scene.setObject( world );

scene.addLight(PointLight(Point(20, -30, 30), Color(2000.9)));
scene.addLight(PointLight(Point(0, 0, 10), Color(200.9)));

scene.setAmbient(Color(0.1))
scene.setCamera(PinholeCamera(Point(0, -60, 12.5),
                              Point(0, 0, 0),
                              Vector(0, 0, 1),
                              50));
scene.setMaxRayDepth(10);
scene.setMinAttenuation(.01);
