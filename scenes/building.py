from Core import *
from Background import *
from Materials import *
from Geometry import *
from IO import *
from Filter import *



squat= None;
talls = []

def fillSquareWithBuildings1( xStart, xEnd, yStart, yEnd ):
  # Building colors
  colors = []
  # Add a bunch of whites
  for i in range(0,10):
    colors.append( Color( 0.5 + drand48() / 2.0 ) )


  # Figure building type
  height = 16
  
  
  # Create window material
  winColor = Color( 0.5 + drand48() / 2.0,
                    0.5 + drand48() / 2.0,
                    0.5 + drand48() / 2.0 )
                    
  winMat = metalMaterial( winColor, 100 )
  
  # Create non-window material
  idxWM = int(drand48() * len(colors))
  if ( idxWM == len(colors) ):
    idxWM -= 1
  nonWinMat = lambertianMaterial( colors[idxWM], 0.6, 0.4 )
  
  # Create a building modulator
  modulator = SimpleBuildingModulator( drand48() * 0.6,
                                       drand48() * 0.4,
                                       5 + 5*drand48(),
                                       height + height*drand48() )
  buildingMat = MaterialModulator( modulator, [nonWinMat, winMat] )


  return Building( buildingMat,
                   nonWinMat,
                   nonWinMat,
                   Point( xStart, yStart, 0 ),
                   Point( xEnd,   yEnd,   height  ) )
 


scene = Scene()
world = Group();
scene.setBackground( GradientBackground( Color (.745, 0.462, 0.313 ),
                                         Color (0,0,1),
                                         False, 3.0 ) )
g = fillSquareWithBuildings1( -20, 20,
                              0, 20 )
world.addObject( g )
scene.setObject( world );

scene.addLight(PointLight(Point(0, -20, 20), Color(.9)));
#scene.addLight(DirectionalLight( Vector(0.426, -0.639, 0.639),
#                                 Color( 1, 0.9, 0.7) ) )

scene.setCamera(PinholeCamera(Point(0, -60, 22.5),
                              Point(0, 0, 0),
                              Vector(0, 0, 1),
                              55));
scene.setMaxRayDepth(10);
scene.setMinAttenuation(.01);

