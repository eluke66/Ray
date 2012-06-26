from Core import *
from Background import *
from Materials import *
from Geometry import *
from IO import *
from Filter import *

BuildingFactory.Initialize( 0.1, 30 )
def fillSquare( xStart, xEnd, yStart, yEnd ):
  return BuildingFactory.CreateBuilding( BuildingFactory.BRICK_BUILDING,
                                         Point(xStart,yStart,0),
                                         Point(xEnd,yEnd,0),
                                         0.5 )



scene = Scene()
world = Group();
scene.setBackground( GradientBackground( Color (.745, 0.462, 0.313 ),
                                         Color (0,0,1),
                                         False, 3.0 ) )

g = fillSquare( -20, 20, 0, 20 )
world.addObject( g )
world.addObject( Plane( lambertianMaterial( Color(0.3, 0.2, 0.4), 0.6, 0.2 ),
                        Vector(0,0,1), Point( 0,0,0 ) ) )

scene.setObject( world );
scene.addLight(PointLight(Point(0, -20, 10), Color(414.5)));
scene.setCamera(PinholeCamera(Point(20, -60, 22.5),
                              Point(0, 0, 0),
                              Vector(0, 0, 1),
                              55));
scene.setMaxRayDepth(10);
scene.setMinAttenuation(.01);
scene.setAmbient(Color(0.9))
