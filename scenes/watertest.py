from Core import *
from Background import *
from Materials import *
from Geometry import *
from IO import *
from Filter import *
import os
import scenes

# SCALE:
# 1 = 1 foot
floorHeight = 600


def bg4():
    mg = MultiGradientColor(False)
    pt = 0.45
    mg.addPoint(0.3, Color(0))
    #mg.addPoint( pt, Color(1) )
    pt += 0.03 ; mg.addPoint( pt, Color(168/255.,221/255.,231/255.) )
    pt += 0.02 ; mg.addPoint( pt, Color(87/255.,151/255.,231/255.) )
    
    pt += 0.05 ; mg.addPoint( pt, Color(26/255.,45/255.,222/255.) )

    return MultiGradientCloudBackground( mg,  SolidColor(Color(1)), 0.4 )


def setupScene(world, camera):
    global floorHeight
    scene = Scene()

    scene.setBackground( bg4() )
   
    scene.setMaxRayDepth(10)
    scene.setMinAttenuation(.01)
    scene.setAmbient(Color(0.1))
    scene.setObject( world )
    scene.setCamera( camera )
    scene.addLight(DirectionalLight( Vector(0.426, -0.639, 0.639),
                                     Color( 1.9 ) ) )                                                 

    # Main light
    scene.addLight( PointLight( Point(0,0,floorHeight + 8-1e-2), Color(128) ) )

    return scene



##################################################################
import city_utils

def makeRiver(xMin, xMax, yMin, yMax,n=5):
    hfmatl = dielectricMaterial( 1.333, Color(0,0,1), 100 );
#    hfmatl = basicShade(Color(1,0,0))
    oceanGroup = GriddedGroup(n)
    
    
    nTilesY = 100;
    nTilesX = 100;
    p = Point( xMin, yMin, 0 );
    tileSizeX = (xMax-xMin) / float(nTilesX);
    tileSizeY = (yMax-yMin) / float(nTilesY);

    h = Water( hfmatl,
               512,
               True, False,
               265 * 3.1415 / 180,
               .00000775, 9.0,
               p, p+Vector(tileSizeX,tileSizeY,0) )
    scenes.spreadWater( oceanGroup, h, Point(xMin,yMin,0), Point(xMax,yMax,0), nTilesX, nTilesY )
    return oceanGroup

 
##################################################################

world = Group()

### TEMPORARY PLACEHOLDERS##################################
world.addObject( makeRiver( -5000, 5000, 0, 10000 ) )
#ocean
#
noWaves = False
if (noWaves):
    omatl = dielectricMaterial( 1.333, Color(0,0,1), 100 )
    bp = BoundedPlane( omatl,
                       Vector(0,0,1), Point(0,0,0.15),
                       Point( -1000000, 10000, 0.1 ),
                       Point(  1000000, 100000000, 0.2 ) )
    bp.bump(RandomBump(0.005))
    world.addObject( bp )
else:
    
    world.addObject(makeRiver(-10000,10000,10000,20000, 5 ) )
    #world.addObject(makeRiver( -1000, 1000, 10000, 20000 ) )
    

#

camera = PinholeCamera(Point(5, 0, floorHeight+4),
                       Point(0, 10000, 0),
                       Vector(0, 0, 1),
                       55 )
                       # 80) at 1600 x 950


scene = setupScene( world, camera )


