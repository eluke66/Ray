from Core import *
from Background import *
from Materials import *
from Geometry import *
from IO import *
from Filter import *


  
def all(world):

  # Should see this
  p = Plane( lambertianMaterial( Color(0,1,0), 0.6, 0.4 ),
             Vector(0,0,1), Point(0,0,0) )

  #### Now do a CSG test, a box with bounding plane
  world.addObject(Box( lambertianMaterial( Color(0,1,1), 0.6, 0.4 ),
                       Point(-15,-25,0), Point(-5,-15,5) ))

  c = CSG(None)
  box = Box( lambertianMaterial( Color(0,1,1), 0.6, 0.4 ),
             Point(5,-25,0), Point(15,-15,5) )
  p = CSG_Primitive(box)

  
  n  = -Vector(0,2,-1)
  n.normalize()
  pl = BoundedPlane( None,
                     n, Point( 0, -23, 8), Point(-6,-30,0), Point(25,-5,6) )
  pp = CSG_And(CSG_Primitive(pl),p)
  
  n  = Vector(0,2,1)
  n.normalize()
  pmatl = checkerMaterial( Point(0,0,0), Vector(0.5,0.5,0), Vector(-0.5,0.5,0),
                           lambertianMaterial(Color(0.0),0.6, 0),
                           lambertianMaterial(Color(1.0),0.6, 0) )
  
  
  pl = BoundedPlane( pmatl,
                     n, Point( 0, -17, 8),
                     Point(4,-25,0), Point(16,-10,6) )
  
  pp = CSG_And(CSG_Not(CSG_Primitive(pl)),pp)

  notbox = Box( None,
                Point( 4.8, -23, -0.1),
                Point( 15.1, -17, 3 ) )
  pp = CSG_And(CSG_Not(CSG_Primitive(notbox)),pp)
  
  notbox = Box( None,
                Point( 7, -25.1, -0.1),
                Point( 13, -14.9, 3 ) )
  pp = CSG_And(CSG_Not(CSG_Primitive(notbox)),pp)
  c.setCSG(pp)
  world.addObject(c)
  
##########################
  
scene = Scene()
world = Group();
scene.setBackground( ConstantBackground( Color (.745, 0.462, 0.313 ) ) )

all(world)

scene.setObject( world );

scene.addLight(PointLight(Point(20, -30, 30), Color(500.9)));
scene.addLight(PointLight(Point(0, 0, 10), Color(500.9)));

scene.setAmbient(Color(0.04))
scene.setCamera(PinholeCamera(Point(0, -60, 12.5),
                              Point(0, 0, 0),
                              Vector(0, 0, 1),
                              50));
scene.setMaxRayDepth(10);
scene.setMinAttenuation(.01);
