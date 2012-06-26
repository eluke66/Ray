from Core import *
from Background import *
from Materials import *
from Geometry import *
from IO import *
from Filter import *

scene = Scene()
world = Group()
scene.setBackground( ConstantBackground( Color (.745, 0.462, 0.313 ) ) )

### WITH MORTAR
bc = BrickColor( Color(1,0,0), Color(0.1,0,0),
                 False, Point2D(0.95,0.8),
                 Color(1,1,1),5,20 )
mat = lambertianBrickMaterial( bc, 0.6, 0.4 )

b = Box( mat, Point(-17,-10,-10), Point(-7,10,10) )
b.bump( MakeBrickBump( bc ) )
world.addObject(b)
#############################

### NO MORTAR - LIKE GRANITE
bc1 = BrickColor( Color(0.9), Color(0.7),
                  False, Point2D(0.98,0.98),
                  Color(0.0),3,7,True)
mat1 = lambertianBrickMaterial( bc1, 0.6, 0.4 )

b1 = Box( mat1, Point(-2,-10,-10), Point(8,10,10) )
b1.bump( MakeBrickBump( bc1 ) )
world.addObject(b1)
#############################
#############################

### NO MORTAR - LIGHT GRANITE
bc2 = BrickColor( Color( 234/255., 197/255., 139/255. ),
                  Color( 168/255., 142/255., 100/255. ),
                  False, Point2D(0.93,0.93),
                  Color(0.5),3,7,True)
mat2 = lambertianBrickMaterial( bc2, 0.6, 0.4 )

b2 = Box( mat2, Point(10,-10,-10), Point(20,10,10) )
b2.bump( MakeBrickBump( bc2 ) )
world.addObject(b2)
#############################

scene.setObject( world );

scene.addLight(PointLight(Point(20, -30, 30), Color(1000.9)));
scene.addLight(PointLight(Point(-20, -40, 20), Color(1000.9)));

scene.setAmbient(Color(0.4))
scene.setCamera(PinholeCamera(Point(0, -60, 12.5),
                              Point(0, 0, 0),
                              Vector(0, 0, 1),
                              50));
scene.setMaxRayDepth(10);
scene.setMinAttenuation(.01);
