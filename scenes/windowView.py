from Core import *
from Background import *
from Materials import *
from Geometry import *
from IO import *
from Filter import *
import os, math
import scenes

def uniform(a,b):
    return a+(b-a)*drand48()

    
# SCALE:
# 1 = 1 foot
floorHeight = 600

##################################################################
def bg3():
    mg = MultiGradientColor(False)
    pt = 0.45
    mg.addPoint( 0.3, Color(0) )
    mg.addPoint( pt, Color(1) )
    pt += 0.03 ; mg.addPoint( pt, Color(1,0.8,0.8) )
    pt += 0.005 ; mg.addPoint( pt, Color(1,0.7,0.3) )
    pt += 0.01 ; mg.addPoint(pt, Color(0.8,0.5,0.6) )
    pt += 0.015 ; mg.addPoint( pt, Color(0.4,0.2,0.8) )
    pt += 0.025 ; mg.addPoint( pt, Color(0.2,0.1,0.8) )
    pt += 0.025 ; mg.addPoint( pt, Color(0.1,0.05,0.6) )
    #mg.addPoint( 1.0, Color(0) )

    return MultiGradientCloudBackground( mg,  SolidColor(Color(1)), 0.4 )
    #return MultiGradientBackground( mg )

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

    day = True

    if ( day ):
        scene.setBackground( bg4() )
        scene.addLight(DirectionalLight( Vector(-0.426, 0.639, -0.639),
                                     Color( 1.9 ) ) )
    else:
        m = lambertianImageMaterialTC(TextureColor("textures/moonbright.jpg"), 0.6, 0.4)
        #moonCenter = Point( -5, 100000, 10000 )
        moonDist = 1.26115814e9
        moonRadius = 5700131.23
        moonPower = 1e5
        moonAngle = 0.34906585 # 20 degrees
        
        world.addObject( SpherePolar(m,
                                     moonCenter, 5000,
                                     Vector(0,0,1), Vector(0,1,0) ) )
        scene.addLight(PointLight( moonCenter,
                                   Color(1000000.0) ))
        scene.addLight(DirectionalLight( Vector(-0.426, 0.639, -0.639),
                                     Color( 1.9 ) ) )
        scene.setBackground( StarfieldBackground(Color(0.0)) )
        
    #gc = GradientColor(Color(0.3,0,0.8), Color(0,0,0.5), False )
    #scene.setBackground( GradientCloudBackground( gc,
    #                                              SolidColor(Color(1)),
    #                                              0.4 ) )
    
    scene.setMaxRayDepth(10)
    scene.setMinAttenuation(.01)
    scene.setAmbient(Color(0.0))
    scene.setObject( world )
    scene.setCamera( camera )
    

    # Main light
    scene.addLight( PointLight( Point(0,0,floorHeight + 8-1e-2), Color(128) ) )

    return scene

##################################################################

def makeRoom():
    global floorHeight
    roomWidth  = 9  # Actually double this
    roomLength = 10 # Actually double this
    roomHeight = 8
    room = Group()

    # Floor
    if ( False ):
        floorMatl = checkerMaterial(Point(0,0,0), Vector(0.5,0.5,0), Vector(-0.5,0.5,0),
                                    phongMaterial(Color(1, 1, 1), 1., 0.4, Color(1,1,1), 60),
                                    phongMaterial(Color(0, 0, 0), 0.6, 0.4, Color(1,1,1), 60));
    else:
        floorMatl = repeatingLambertianImageMaterialTC( TextureColor( "textures/Office-Carpet51.jpg", roomWidth, roomLength),
                                                        roomWidth, roomLength, 0.5, 0.4 )
        
    floor = BoundedPlane( floorMatl,
                          Vector(0,0,1), Point(0,0,floorHeight),
                          Point(-roomWidth, -roomLength, floorHeight-1),
                          Point(roomWidth, roomLength, floorHeight+1) )
    floor.bump( RandomBump( 0.1 ) )
    room.addObject(floor)

    # Ceiling
    cHeight = floorHeight + roomHeight
    ceilingMatl = lambertianMaterial( Color(1,1,1), 0.6, 0.4 )
    ceiling = Box( ceilingMatl,
                   Point(-roomWidth, -roomLength, cHeight),
                   Point(roomWidth, roomLength, cHeight+1) )
    room.addObject(ceiling)

    # L and R walls
    bc = BrickColor( Color(1,0,0), Color(0.1,0,0),
                     True, Point2D(0.8,0.8),
                     Color(1,1,1),50,20 )
    wallMatl = lambertianBrickMaterial( bc, 0.6, 0.4 )
    
    #wallMatl = lambertianMaterial( Color(0.8, 0.1, 0.2), 0.4, 0.2 )
    b = Box( wallMatl,
             Point( -roomWidth-1, -roomLength, floorHeight ),
             Point( -roomWidth, roomLength, cHeight ) )
    bb = MakeBrickBump( bc )
    #b.bump( bb )

    room.addObject( b )
    b = Box( wallMatl,
             Point( roomWidth, -roomLength, floorHeight ),
             Point( roomWidth+1, roomLength, cHeight ) )
    b.bump( bb )
    
    room.addObject( b )

    # Window (is this temporary?!)
    #windowMatl = transparentMaterial( Color(1,1,1), 1.0 )
    #windowMatl = dielectricMaterial( 1.333, Color(1,1,1), 1000000 )
    windowMatl = dielectricMaterial( 1, Color(1), 1000000)
    dx = 0.5 / 12.0 # Half inch thick glass
    #room.addObject( Box( windowMatl,
    #                     Point( -roomWidth, roomLength - dx, floorHeight ),
    #                     Point( roomWidth, roomLength + dx, cHeight ) ) )
    #                     
    #room.addObject( BoundedPlane( windowMatl,
    #                              Vector( 0, -1, 0 ), Point( 0, roomLength, 0 ),
    #                              Point( -roomWidth, roomLength - 1, floorHeight ),
    #                              Point( roomWidth, roomLength + 1, cHeight ) ) )

    # Back wall
    wallMatl = lambertianMaterial( Color(0.8, 0.1, 0.2), 0.4, 0.2 )
    room.addObject( Box( wallMatl,
                         Point( -roomWidth, -roomLength - 1, floorHeight ),
                         Point( roomWidth, -roomLength, cHeight ) ) )

    # Pillar
    pillarMatl = ceilingMatl # For now
    pillarWidth = 0.25       # Actually double
    pillar = Box( pillarMatl,
                  Point( roomWidth*0.25 - pillarWidth,
                         roomLength - 2*pillarWidth,
                         floorHeight ),
                  Point( roomWidth *0.25 + pillarWidth,
                         roomLength,
                         cHeight ) )
    room.addObject(pillar)

    
    

    # Table
    # PLACEHOLDER!
    twidth = 4
    tlength = 1.5
    theight = 1
    tFromWindow = 1
    tFromWall = 1
    tableStartX = roomWidth - twidth - tFromWall
    tableStartY = roomLength - tFromWindow - tlength
    tmatl = phongWoodMaterial(Color(227, 210, 187)/255.,
                              Color(181, 160, 117)/255.,
                              120, 3,  5, .04, 1, .1, 2, 0.6, 10,
                              0.5, 0.5, Color(0,0,0), 120)
    
    p1 = Point( tableStartX,
                tableStartY,
                floorHeight )
    p2 = Point( roomWidth - tFromWall,
                roomLength - tFromWindow,
                floorHeight + theight )
    table = Box( tmatl, p1, p2 )
    #table.bump(  MapBump(TextureColor("textures/bump.ppm",100,100)))
    #table.bump( WoodBump( Color(227, 210, 187)/255.,
    #                      Color(181, 160, 117)/255.,
    #                      120, 3,  5, .04, 1, .1, 2, 0.6, 10 ) )
    room.addObject(table)


    # Buddha
    buddha = ObjectLoader.LoadObject( "Objects/happy_vrip_res4.ply", "" )
    greenMarble = phongMarbleMaterial(Color(0.0, 1, 0.0), Color(1, 1, 1.0),
                                                   0.5, 
                                                   8,
                                                   20,
                                                   1, 2, 
                                                   0.6,
                                                   0.6, 0.4, Color(1,1,1), 120)
    buddha.setMaterial(greenMarble)
    p = Position()
    p.translate( Vector( tableStartX + 0.5, tableStartY+ 0.5, theight+floorHeight+.45), False );
    p.rotate( Vector(270,0,0) )
    p.scale( 0.15, Position.SCALE_MAX )
    p.transform(buddha);
    room.addObject( buddha );

    # Dragon
    dragon = ObjectLoader.LoadObject( "Objects/dragon_vrip_res4.ply", "" )
    redMarble = phongMarbleMaterial(Color(1.0, 0, 0.0), Color(1, 1, 1.0),
                                    0.5, 
                                    8,
                                    20,
                                    1, 2, 
                                    0.6,
                                    0.6, 0.4, Color(1,1,1), 120)
    dragon.setMaterial(redMarble)
    p = Position()
    p.translate( Vector( tableStartX - 0.5 + twidth/2, tableStartY+ 0.5, theight+floorHeight+0.27), False );
    p.rotate( Vector(270,-30,0) )
    p.scale( 0.15, Position.SCALE_MAX )
    p.transform(dragon);
    room.addObject( dragon );

    # Argonath
    argo = ObjectLoader.LoadObject( "Objects/Argonath.3ds", "" )
    p = Position()
    p.translate( Vector( tableStartX + 0.76 + twidth/2,
                         tableStartY+ 0.5,
                         theight+floorHeight+0.05 ), True )
    p.rotate( Vector(0,90,0) )
    p.scale( 0.15, Position.SCALE_MAX )
    p.transform(argo)
    room.addObject(argo)
    
    # Glass balls
    radius = 1.0 / 12.0
    otherPoints = []
    for i in range(0,10):
        glass =  dielectricMaterial( 1.333, Color(1,0,0), 100 )
        okay = False
        while (not okay):
            okay = True
            pt =  Point( tableStartX + uniform(-3,-0.5),
                         tableStartY + uniform(-.2,1),
                         floorHeight+radius)
            for opt in otherPoints:
                if ( (pt-opt).length() < 3 * radius ):
                    okay = False
                    break
        
        room.addObject( Sphere( glass, pt, radius ) )
        otherPoints.append(pt)

    qaeb = SinQAEB(dielectricMaterial( 1.333, Color(1,1,0), 100 ),
                   Point( tableStartX+3, tableStartY, floorHeight),
                   Point( tableStartX+4.0, tableStartY - 1, floorHeight+1),
                   0.5 * math.tan(math.radians(20)) / 16 )
    p1 = CSG_Primitive(qaeb)
    cyl = Cylinder(redMarble, 2, 0.5)
    p.reset()
    p.translate( Vector( tableStartX+3.5, tableStartY-0.5, floorHeight ), True )
    p.transform(cyl)
    p2 = CSG_Primitive(cyl)
    union = CSG_And(p1,p2)
    c = CSG(None)
    c.setCSG(union)
    #room.addObject(c)
    
    #room.addObject(qaeb)
                   
    
    # Dartboard
    dartboard = ObjectLoader.LoadObject( "Objects/Models/Models/Household/DARTBORD.3DS", "" )
    p = Position()
    p.translate( Vector( roomWidth-.5, roomLength - 1.5, floorHeight + 4 ), True )
    p.rotate( Vector( 0,0,90) )
    p.scale( 0.08, Position.SCALE_MAX )
    p.transform(dartboard)
    room.addObject( dartboard )

    # Wall light
    wallLight = ObjectLoader.LoadObject( "Objects/Models/Models/Household/LANTERN2.3DS", "" )
    p = Position()
    p.translate( Vector( roomWidth-.5, roomLength - 1.5, floorHeight + 6 ), True )
    p.scale( 2, Position.SCALE_MAX )
    p.transform(wallLight)
    room.addObject( wallLight )

    return room

    

##################################################################
import city_utils

def makeRiver(xMin, xMax, yMin, yMax,n=5):
    hfmatl = dielectricMaterial( 1.333, Color(0,0,1), 100 );
    oceanGroup = GriddedGroup(n)
    
    
    nTilesY = 200;
    nTilesX = 100;
    p = Point( xMin, yMin, 0 );
    tileSizeX = (xMax-xMin) / float(nTilesX);
    tileSizeY = (yMax-yMin) / float(nTilesY);

    h = Water( hfmatl,
               512,
               True, False,
               265 * 3.1415 / 180,
               .00000775, 2.0,
               p, p+Vector(tileSizeX,tileSizeY,0) )
    scenes.spreadWater( oceanGroup, h, Point(xMin,yMin,0), Point(xMax,yMax,0), nTilesX, nTilesY, 0.0 )
    return oceanGroup

 
def makeCity(xMin, xMax, yMin, yMax):
    HAVE_WATER = True
    cityGroup = Group()
    ground = None
    
    if ( HAVE_WATER ):
        #groundMatl = lambertianMaterial( Color( 142/255., 161/255., 22/255. ), 0.6, 0.4 )
        groundMatl = lambertianMaterial( Color( 29/255., 89/255., 0. ), 0.6, 0.4 )
        ground = Mountains( groundMatl,
                            Point(xMin, yMin, -30),
                            Point(xMax, yMax, 250),drand48(),drand48(),False )
        cityGroup.addObject(ground)
        cityGroup.addObject( makeRiver( xMin*2, xMax*2, yMin, yMax*2) )
        
    city_utils.createCityBuildings( cityGroup, xMin, xMax, yMin, yMax, ground )
    return cityGroup
    
##################################################################
def makeMountains():
    mtnMatl = lambertianMaterial( Color(0.45, 0.235, 0.105), 0.6, 0.4 )
    mtns = Mountains( mtnMatl,
                      Point(-10000, -15000, -2500),
                      Point( 10000, -5000, 5000),0,0,False )
    return mtns


##################################################################

world = Group()

### TEMPORARY PLACEHOLDERS##################################
#ground.
#world.addObject( Plane( lambertianMaterial( Color( 1, 1, 0 ), 0.6, 0.4 ),
#                        Vector(0,0,1), Point(0,0,0) ) )

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
    #world.addObject(makeRiver(-30000,30000,10000,40000, 5 ) )
    world.addObject(BoundedPlane( basicShade(Color(0)),
                                  Vector(0,0,1), Point(0,0,0),
                                  Point( -30000,10000,-50 ),
                                  Point(30000,40000,-49) ) )
    

# Fog
#fog2 = ConstantFogCalculator(0.005)
#fmatl = fog(1, Color(0.8), 100, fog2 )
#world.addObject( Box(fmatl,
#                     Point( -1000000, 10001, 0 ),
#                     Point(  1000000, 100000, 500 ) ) )
#world.addObject( Box(fmatl,
#                     Point( -1000000, 100000, 0 ),
#                     Point(  1000000, 1000000000, 5000 ) ) )

########## CITY FOG
if ( True ):
    fog2 = FogCombiner( LinearFogCalculator(Vector(0,0,1), 1),
                        PerlinFogCalculator(4, 0.5, 0.5),
                        0.2, 4.0 )
    fog2 = LinearFogCalculator( Vector(0,0,1), 1 )
    
    fmatl = fog(0.01, Color(0.8), 100, fog2 ) 
    world.addObject( Box(fmatl,
                     Point( -1000000, 0, 5 ),
                     Point(  1000000, 10000, 500 ) ) )
    
#world.addObject( Plane( lambertianMaterial( Color(0.1,0.05, 0.01), 0.6, 0.4 ),
#                        Vector(0,0,1), Point( 0, 0, -20 ) ) )
#world.addObject( BoundedPlane( lambertianMaterial( Color(0,0,1), 0.6, 0.4 ),
#                               Vector(0,0,1), Point(0,0,0.15),
#                               Point( -1000000, 10000, 0.1 ),
#                               Point(  1000000, 10000000, 0.2 ) ) )

############################################################

world.addObject( makeRoom() )
world.addObject( makeCity( -5000, 5000, 0, 10000 ) )
world.addObject( makeMountains() )

# Orig eyePoint = Point(5, 0, floorHeight+4)
eyePoint = Point(5, 350, 500)
camera = PinholeCamera( eyePoint,
                       Point(0, 10000, 0),
                       Vector(0, 0, 1),
                       55 )
                       # 80) at 1600 x 950


scene = setupScene( world, camera )



m = PinholeCameraMover()


#m.addPoint( Point( 0, 10, 0 ), True, Point(0.0,0.0,0.0), False, Vector(0.0,0.0,0.0), False )
#m.addPoint( Point( 0, 0, -floorHeight+70 ), True, Point(0.0,0.0,0.0), False, Vector(0.0,0.0,0.0), False )
#
#for i in range(0,20):
#    m.addPoint( Point( 0, 100, 0 ), True, Point(0.0,0.0,0.0), False, Vector(0.0,0.0,0.0), False )

pt1 = EAU()
pt1.setEye( Point( 0, 10, 0 ) )
m.addPoint( pt1 )

pt2 = EAU()
pt2.setEye( Point( 0, 0, -floorHeight+70 ), 1, 32 )
m.addPoint( pt2 )

pt3 = EAU()
pt3.setEye( Point( 0, 5280, 0 ) )
m.addPoint(pt3)

scene.addFrameManipulator(m)
