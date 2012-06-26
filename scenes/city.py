from Core import *
from Background import *
from Materials import *
from Geometry import *
from IO import *
from Filter import *

# TODO:
#
# 
#
#
#
#
#

def makeGround():
  world = Group();

  ####################################################
  # Grass ground
  tc = TextureColor( "textures/grass.jpg" )
  g = lambertianImageMaterialTC( tc, 0.6, 0.4 )
  #world.addObject( Plane( g, Vector(0,0,1), Point(0,0,0) ) );
  world.addObject( BoundedPlane( g, Vector(0,0,1), Point(0,0,0),
                                 Point(-1000, -2, -1), Point(1000, 10, 1) ) )
  
  ####################################################
  # Fog
  fog1 = PerlinFogCalculator(4, 0.5, 0.5)
  fog2 = LinearFogCalculator(Vector(0,0,1), 1)
  fmatl = fog(1, Color(1,1,1), 10, FogCombiner(fog2,fog1,1,0.1) )
  world.addObject( Box( fmatl ,
                        Point(-400, -10, -0.001), Point( 400, 400, 2 ) ) )
  
  ####################################################
  # Ocean
  hfmatl = repeatingPhongImageMaterial( "textures/waterblue.ppm",5.3,4.7,
                               0.6, 0.4,
                               Color(0,0,1),
                               100 )

  
  #hfmatl = dielectricMaterial( 1.333, Color(0,0,1), 100 );

  oceanGroup = GriddedGroup(5)
  nTilesY = 200;
  nTilesX = 40;
  p = Point( -1280.0, -4000.0, 0 );
  tileSizeX = 2048.0 / float(nTilesX);
  tileSizeY = 4000 / float(nTilesY);

  h = Water( hfmatl,
             512,
             True, False,
             265 * 3.1415 / 180,
             .00000775, 3.0,
             p, p+Vector(tileSizeX,tileSizeY,0) )
  fudge = 0.01;
  waterMin = h.getMinimum();

  for x in range(0,nTilesX):
      for y in range(0,nTilesY):
          pos = Position();
          pos.translate( Vector(x*(tileSizeX-fudge), y*(tileSizeY-fudge), -waterMin ), False );
          i = Instance( pos.getMatrix( h ),
                        h,
                        hfmatl );
          oceanGroup.addObject(i);

  lastWater = Box( hfmatl,
                   Point(-20000,-20000,0.1),
                   Point(20000,-4000,0.2) )

  oceanGroup.addObject( lastWater )
  world.addObject(oceanGroup)

  
  
  ####################################################
  # Add mountains
  mtnMatl = lambertianMaterial( Color(0.45, 0.235, 0.105), 0.6, 0.4 )
  mtns = Mountains( mtnMatl,
                    Point(-320, 280, -7),
                    Point( 320, 440, 60 ),0,0,False )
  world.addObject(mtns)

  world.addObject( Mountains( mtnMatl,
                              Point(-320, 440, -7),
                              Point( 320, 600, 80 ),
                              1.3, 10.1, False) )
  world.addObject( Mountains( mtnMatl,
                              Point(-400, 600, -7),
                              Point( 400, 600, 220 ),
                              5.3, 5.5, False) )
  return world;


squat= None;
talls = []
BuildingFactory.Initialize( 0.1, 33 )

def fillSquareWithBuildings1( xStart, xEnd, yStart, yEnd ):
  
  return BuildingFactory.CreateBuilding( BuildingFactory.RandomBuilding(),
                                         Point(xStart,yStart,0),
                                         Point(xEnd,yEnd,0),
                                         0.5 )

  # Building colors
  colors = [ Color(0.647059, 0.164706, 0.164706),
             Color(0.419608, 0.137255, 0.556863),
             Color(0.184314, 0.309804, 0.309804),
             Color(0.329412, 0.329412, 0.329412),
             Color(0.556863, 0.137255, 0.137255),
             Color(0.184314, 0.184314, 0.309804),
             Color(0.137255, 0.419608, 0.556863) ]
  # Add a bunch of whites
  for i in range(0,10):
    colors.append( Color( 0.5 + drand48() / 2.0 ) )


  # Figure building type
  bType = drand48()

  if ( bType > 0.5 ):
    height = 3.0 + 3*drand48()
  elif ( bType > 0.75 ):
    height = 3.0 + 8*drand48()
  elif ( bType > 0.87 ):
    height = 3.0 + 16*drand48()
  elif ( bType > 0.94 ):
    height = 3.0 + 24*drand48()
  else:
    height = 3.0 + 30*drand48()
  
    #height = 3.0 + 18*drand48()
  
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
                   Point( xStart, yStart, 0 ),
                   Point( xEnd,   yEnd,   height  ) )
  #return Box( buildingMat,
  #            Point( xStart, yStart, 0 ),
  #            Point( xEnd,   yEnd,   height  ) );

import os
def verifyFile(name):
    try:
        os.stat(name)
    except OSError:
        return False
    return True
    
def fillSquareWithBuildings( xStart, xEnd, yStart, yEnd ):
    global squat, talls

    height = 3.0 + 18*drand48();
    width = (xEnd - xStart);

    if ( len(talls) == 0 ):
        i = 0
        while (True): 
            ostr = "textures/tallBuilding" + str(i) + ".jpg"
            if ( verifyFile( ostr ) ):
                talls.append( repeatingPhongImageMaterial( ostr, 1.0, 2.0, 0.6, 0.4, Color(0,0,0), 80) );
            else:
                break
            i = i+1
        print "Found ",len(talls), " building textures. "
    
    
    if ( squat is None ):
        squat = phongImageMaterial("textures/shortBuilding.jpg", 0.6, 0.4, Color(0,0,0), 80);
    y = None
    if ( height > width ):
        idx = int(drand48() * len(talls))
        if ( idx == len(talls) ):
            idx -= 1
        y = talls[ idx ];
    else:
        y = squat;

    return Box( y,
                Point( xStart, yStart, 0 ),
                Point( xEnd,   yEnd,   height  ) );

vRoad=None
hRoad=None;

def makeLane( xStart, xEnd, yStart, yEnd, nLanes, horizontal ):
    global hRoad, vRoad
    
    laneSize = 2.0;
    r = None
    if ( horizontal ):
        if ( hRoad is None ):
            hRoad = TextureColor( "textures/Roadh.jpg" );
        r = repeatingLambertianImageMaterialTC(hRoad,
                                               (xEnd - xStart) / laneSize, float(nLanes),
                                               0.6, 0.4)
    
    else:
        if ( vRoad is None ):
            vRoad = TextureColor( "textures/Road.jpg" );
        r = repeatingLambertianImageMaterialTC(vRoad,
                                               float(nLanes), (yEnd - yStart) / laneSize,
                                               0.6, 0.4 )
        
    return Box( r,
                Point( xStart, yStart, 0 ),
                Point( xEnd,   yEnd,   1e-4 ) );


class D2:
    def __init__(self,x=0,y=0):
        self.x = x
        self.y = y
        

def getLanes():
  s = drand48();
  numLanes = 4;
  if ( s <= 0.5 ):
    numLanes = 1;
  elif ( s <= 0.75 ):
    numLanes = 2;
  elif ( s <= 0.9 ):
    numLanes = 3;
  return numLanes;


def createCityBuildings( world ):
    group = GriddedGroup(5);
    start = D2(-230, 10);
    end = D2( 230, 250 );
    avgBlocksize = D2( 10, 10 );
    blockVariance = D2( 5, 5 );
    laneSize = 2.0;
    current = D2();

    xBlockSizes = []
    yBlockSizes = []
    xStreetLanes = []
    yStreetLanes = []
  
    current.x = start.x;
    while ( current.x < end.x ):
        blkSize = avgBlocksize.x + blockVariance.x * (drand48() - 0.5);
        xBlockSizes.append( blkSize );
        numLanes = getLanes();
        xStreetLanes.append( numLanes );
        current.x += blkSize + numLanes * laneSize;
  
  
    current.y = start.y;
    while ( current.y < end.y ):
        blkSize = avgBlocksize.y + blockVariance.y * (drand48() - 0.5);
        yBlockSizes.append( blkSize );
        numLanes = getLanes();
        yStreetLanes.append( numLanes );
        current.y += blkSize + numLanes * laneSize;
      
  
    current.x = start.x;
    blkSize = D2()
    for x in range(0,len(xBlockSizes)):
      current.y = start.y;
      blkSize.x = xBlockSizes[x];
      for y in range(0,len(yBlockSizes)):
        blkSize.y = yBlockSizes[y];

        minInset = 0.5
        insetSize = 2.0

        group.addObject( fillSquareWithBuildings1( current.x + (insetSize*drand48() + minInset),
                                                   current.x + blkSize.x - (insetSize*drand48() + minInset),
                                                   current.y + (insetSize*drand48() + minInset),
                                                   current.y + blkSize.y - (insetSize*drand48() + minInset) ) )
        
        group.addObject( makeLane( current.x,
                                   current.x + blkSize.x,
                                   current.y + blkSize.y,
                                   current.y + blkSize.y + yStreetLanes[y] * laneSize,
                                   yStreetLanes[y],
                                   True ) );
        current.y += blkSize.y + yStreetLanes[y] * laneSize;
        
      group.addObject( makeLane( current.x + blkSize.x,
                                 current.x + blkSize.x + xStreetLanes[x] * laneSize,
                                 start.y,
                                 end.y,
                                 xStreetLanes[x],
                                 False ) );
      current.x += blkSize.x + xStreetLanes[x] * laneSize;

    nShoreLanes = int(3 * drand48()) + 1
    group.addObject( makeLane( start.x, end.x,
                               start.y - nShoreLanes*laneSize, start.y,
                               nShoreLanes,
                               True) )
    world.addObject(group);


import os
scene = Scene()
world = Group();
scene.setBackground( GradientBackground( Color (.745, 0.462, 0.313 ),
                                         Color (0,0,1),
                                         False, 3.0 ) );
world.addObject( makeGround() );

createCityBuildings( world );

scene.setObject( world );

scene.addLight(PointLight(Point(20, -30, 30), Color(.2,.2,.2)));
scene.addLight(DirectionalLight( Vector(0.426, -0.639, 0.639),
                                 Color( 1, 0.9, 0.7) ) )
if ( os.environ.has_key( "MOVE_CAMERA") ):
    scene.setCamera(PinholeCamera(Point(0 + 40*(drand48()-0.5), -40, 7.5 + 5*drand48()),
                                  Point(0, 0, 0),
                                  Vector(0, 0, 1),
                                  55));
  
else:
    scene.setCamera(PinholeCamera(Point(0, -60, 22.5),
                                  Point(0, 0, 0),
                                  Vector(0, 0, 1),
                                  55));
scene.setMaxRayDepth(10);
scene.setMinAttenuation(.01);

if ( os.environ.has_key("Q") ):
  scene.setSampling( Scene.Jittered, 4 )
  scene.setFilter( FilterFactory.findFilter2D("LanczosSinc2") )
