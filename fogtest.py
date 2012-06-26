from Core import *
from Background import *
from Materials import *
from Geometry import *
from IO import *
from Filter import *

scene = Scene()
world = Group();
scene.setBackground( GradientBackground( Color (.745, 0.462, 0.313 ),
                                         Color (0,0,1),
                                         False, 3.0 ) )


#fogmat = fog(1, Color(1,1,1), 10, ConstantFogCalculator(0.02))
#fogmat = fog(1, Color(1,1,1), 10, PerlinFogCalculator(4, 0.5, 0.5))
fogmat = fog(1, Color(1,1,1), 10, LinearFogCalculator(Vector(0,0,1), 1))
#fogmat = fog(1, Color(1,1,1), 10, ExpFogCalculator(-1,Vector(0,0,1), 1))

fog1 = PerlinFogCalculator(4, 0.5, 0.5)
fog2 = LinearFogCalculator(Vector(0,0,1), 1)
fogmat = fog(1, Color(1,1,1), 10, FogCombiner(fog2,fog1,1,0.2) )

b = Box( fogmat, Point(-400, -400, -0.001), Point( 400, 400, 4 ) )
world.addObject(b)
tc = TextureColor( "textures/grass.jpg" )
g = repeatingLambertianImageMaterialTC( tc, 0.6, 0.4 )
world.addObject(Plane( g, Vector(0,0,1), Point(0,0,0) ))

world.addObject(Box( lambertianMaterial(Color(1,0,0), 0.6, 0.4),
                     Point( -3, 0, 0),
                     Point( 4, 10, 7 ) ) )

scene.setObject( world );
scene.setAmbient(Color(.2, .2, .2));

scene.addLight(PointLight(Point(20, -30, 30), Color(.9,.9,.9)))
scene.setCamera(PinholeCamera(Point(0, -60, 22.5),
                                  Point(0, 0, 0),
                                  Vector(0, 0, 1),
                                  55))
scene.setMaxRayDepth(10)
scene.setMinAttenuation(.01)
