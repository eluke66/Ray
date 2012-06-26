from Core import *
from Background import *
from Materials import *
from Geometry import *
from IO import *
from Filter import *


world = Group()
groundMat = lambertianMaterial(Color(1.0),0.6, 0)
world.addObject(BoundedPlane(groundMat, Vector(0,1,0), Point(0,0,0),
                             Point(-1e2,-1,-1e2), Point(1e2,1,1e2) ) )

camEye = Point(0, 10, 0)
camAt  = Point(0, 0, -100 )
camUp  = Vector(0,1,0)
FOV    = 37

camera = PinholeCamera( camEye, camAt, camUp, FOV )

scene = Scene()
scene.addLight( SimpleSpotLight( Vector( 1, -1, -1 ),
                           Point( 0, 5, -40 ),
                           1,
                           Color(1000.0) ) )
scene.addLight( SpotLight( Vector( -1, -1, -1 ),
                           Point( 0, 5, -40 ),
                           10,
                           Color(1000.0),
                           1 ) )

#scene.addLight(PointLight( Point( 0,10,-40 ),
#                           Color(1000.0) ) )

scene.setBackground( ConstantBackground(Color(1,0,0)) )
scene.setMaxRayDepth(10)
scene.setMinAttenuation(.01)
scene.setAmbient(Color(0.1))
scene.setObject( world )
scene.setCamera( camera )


