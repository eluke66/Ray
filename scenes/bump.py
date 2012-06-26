from Core import *
from Background import *
from Materials import *
from Geometry import *
from IO import *
from Filter import *

def makeBump():
  t = TextureColor("textures/bump.ppm")
  return MapBump( t )
  
def all(world):
  p = Plane( lambertianMaterial( Color(1,0,0), 0.6, 0.4 ),
             Vector(0,0,1), Point(0,0,0) )
  p.bump( makeBump() )
  #p.bump( RandomBump( 0.1 ) ) 
  world.addObject(p)
  
##########################
  
scene = Scene()
world = Group();
scene.setBackground( ConstantBackground( Color (.745, 0.462, 0.313 ) ) )

all(world)

scene.setObject( world );

scene.addLight(PointLight(Point(20, -30, 30), Color(.9)));
scene.addLight(PointLight(Point(0, 0, 10), Color(.9)));

scene.setAmbient(Color(0.04))
scene.setCamera(PinholeCamera(Point(0, -60, 12.5),
                              Point(0, 0, 0),
                              Vector(0, 0, 1),
                              50));
scene.setMaxRayDepth(10);
scene.setMinAttenuation(.01);
