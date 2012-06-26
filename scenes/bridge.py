from Core import *
from Background import *
from Materials import *
from Geometry import *
from IO import *
from Filter import *

def Bridge():
  bGroup = Group()
  bridge = ObjectLoader.LoadObject("/home/luke/Desktop/BRIDGE/bridgespan.lwo", "" )
  support = ObjectLoader.LoadObject("/home/luke/Desktop/BRIDGE/bridgesupport.lwo", "" )

  b = BoundingBox()
  bridge.getBounds(b)
  vx = (b.getMax()-b.getMin()).x()

  p = Position()
  p.translate( Vector(-vx/2.,0,0), True )
  p.transform(bridge)
  bGroup.addObject(bridge)

  p = Position()
  p.translate( Vector(0,0,-9), True )
  p.transform(support)
  bGroup.addObject(support)
  
  
  #for i in range(1,3):
  #  p = Position()
  #  p.translate( Vector( (i*vx), 0, 0 ), True )
  #  i = Instance( p.getMatrix(bridge),
  #                bridge )
  #  bGroup.addObject(i)

  
#  p = Position()
#  p.translate( Vector(0,0,-5), True )
#  p.rotate( Vector(0,0,0) )
#  p.transform(bridge1)

#  bGroup.addObject(bridge)
#  bGroup.addObject( bridge1 )
  return bGroup


scene = Scene()
world = Group();
scene.setBackground( ConstantBackground( Color (.745, 0.462, 0.313 ) ) )

world.addObject(Bridge())

scene.setObject( world );

scene.addLight(PointLight(Point(0, -30, 30), Color(1000)));
scene.addLight(PointLight(Point(0, 0, 20), Color(100)));

scene.setAmbient(Color(0.2))
scene.setCamera(PinholeCamera(Point(0, -40, 12.5),
                              Point(0, 0, 0),
                              Vector(0, 0, 1),
                              50));
scene.setMaxRayDepth(10);
scene.setMinAttenuation(.01);
