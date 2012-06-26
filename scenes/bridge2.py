from Core import *
from Background import *
from Materials import *
from Geometry import *
from IO import *
from Filter import *

import scenes


hBridge = True

def bg4():
    mg = MultiGradientColor(False)
    pt = 0.45
    mg.addPoint(0.3, Color(0))
    #mg.addPoint( pt, Color(1) )
    pt += 0.03 ; mg.addPoint( pt, Color(168/255.,221/255.,231/255.) )
    pt += 0.02 ; mg.addPoint( pt, Color(87/255.,151/255.,231/255.) )
    
    pt += 0.05 ; mg.addPoint( pt, Color(26/255.,45/255.,222/255.) )

    return MultiGradientCloudBackground( mg,  SolidColor(Color(1)), 0.4 )

scene = Scene()
world = Group();
#scene.setBackground( ConstantBackground( Color (.745, 0.462, 0.313 ) ) )
scene.setBackground( bg4() )


hRoad = TextureColor( "textures/Roadh.jpg" );
r = repeatingLambertianImageMaterialTC(hRoad,
                                       30, 2,
                                        0.6, 0.4)
vRoad = TextureColor( "textures/Road.jpg" );
rv = repeatingLambertianImageMaterialTC(vRoad,
                                       2, 30,
                                        0.6, 0.4)


#if (hBridge):
#    bridge = Bridge( r, Point(-15,0,-5), Point(15,5,-5), 1, -20, 0.5 )
#else:
#    bridge = Bridge( r, Point(0,-15,-5), Point(5,15,-5), 1, -20, 0.5 )
#world.addObject(bridge)

#world.addObject( PlatformBridge( r, Point(-15,0,-5), Point(15,5,-5), 6, -20 ) )
#world.addObject( PlatformBridge( rv, Point(-20,5,-5), Point(-15,135,-5), 5, -20 ) )

world.addObject( SuspensionBridge( r, Point(-15,0,-5), Point(15,5,-5), 6, -20, 0.5, 4 ) )
world.addObject( SuspensionBridge( rv, Point(-20,5,-5), Point(-15,135,-5), 5, -20, 0.5, 6 ) )

#world.addObject( ArchBridge( r, Point(-15,0,-5), Point(15,5,-5), -3, -20 ) )
#world.addObject( ArchBridge( rv, Point(-20,5,0), Point(-15,135,0), -1, -20 ) )

####### WATER
hfmatl = dielectricMaterial( 1.333, Color(0,0,1), 100 );
#hfmatl = lambertianMaterial( Color(0,0,1), 0.6, 0.4)
oceanGroup = GriddedGroup(10)

xMin = -500
xMax = 500
yMin = -500
yMax = 500

nTilesY = 10;
nTilesX = 10;
p = Point( xMin, yMin, -10 );
tileSizeX = (xMax-xMin) / float(nTilesX);
tileSizeY = (yMax-yMin) / float(nTilesY);

h = Water( hfmatl,
           512,
           True, False,
           265 * 3.1415 / 180,
           .00000775, 2.0,
           p, p+Vector(tileSizeX,tileSizeY,-10) )

scenes.spreadWater( oceanGroup, h, Point(xMin,yMin,-10), Point(xMax,yMax,-10), nTilesX, nTilesY,-8 )
world.addObject(oceanGroup)


xSpan = 5
ySpan = 4
plane = BoundedPlane(lambertianMaterial( Color(0.2,0.2,1), 0.6, 0.4),
                     Vector(0,0,1), Point(0,0,-50),
                     Point(xSpan*xMin,ySpan*yMin,-51),
                     Point(xSpan*xMax,ySpan*yMax,-49) )
world.addObject(plane)
#######



scene.setObject( world );

usePointLight = True
if (usePointLight):
    scene.addLight(PointLight(Point(0, -30, 30), Color(1500)));
else:
    # Plane
    b = BoundedPlane( basicShade(Color(1)),
                      Vector(0, -1, -0.4),
                      Point(0, -500, 100),
                      Point(-100, -499, 0),
                      Point(100, -501, 100) )
    scene.addLight( AreaLight(b, Color(150000), 40) )
    
# Directional
#v = Vector(-1, -1, -0.2)
#v.normalize()
#scene.addLight( DirectionalLight(v, Color( 1)) )



#scene.addLight(PointLight(Point(0, 0, 20), Color(100)));

scene.setAmbient(Color(0.2))
if ( hBridge ):
    scene.setCamera(PinholeCamera(Point(0, -40, 5.5),
                                  Point(0, 0, 0),
                                  Vector(0, 0, 1),
                                 50))
# scene.setCamera(PinholeCamera(Point(20, 40, 0.5),
#                               Point(-10, 40, 0),
#                               Vector(0, 0, 1),
#                               50))
 
else:
    scene.setCamera(PinholeCamera(Point(40, -10, 10.5),
                                  Point(0, 0, 0),
                                  Vector(0, 0, 1),
                                  50))
scene.setMaxRayDepth(10);
scene.setMinAttenuation(.01);
