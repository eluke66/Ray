from Core import *
from Background import *
from Materials import *
from Geometry import *
from IO import *
from Filter import *

def makeGround():
  world = Group();

  ####################################################
  # Grass ground
  tc = TextureColor( "textures/grass.jpg" )
  g = lambertianImageMaterialTC( tc, 0.6, 0.4 )
  world.addObject( BoundedPlane( g, Vector(0,0,1), Point(0,0,0),
                                 Point(-1000, -200, -1), Point(1000, 1000, 1) ) )
                                 
  
  
  return world;


cityMod = None
def makeGroundNew():
  global cityMod
  world = Group();

  ####################################################
  # Grass ground
  grass = TextureColor( "textures/grass.jpg" )
  g = lambertianImageMaterialTC( grass, 0.6, 0.4 )

  hRoad = TextureColor( "textures/Roadh.jpg" );
  rh = repeatingLambertianImageMaterialTC(hRoad,
                                          1, 1,
                                          0.6, 0.4)
  
  vRoad = TextureColor( "textures/Road.jpg" );
  rv = repeatingLambertianImageMaterialTC(vRoad,
                                          1, 1,
                                          0.6, 0.4)

  mats = [ g, rh, rv, basicShade(Color(1,0,0)), basicShade(Color(0,0,1)) ]

  cityMod = CityGridModulator( Point2D(-230, -50), Point2D(230, 250), 5 )
  mmod = MaterialModulator( cityMod, mats )
  
  world.addObject( BoundedPlane( mmod, Vector(0,0,1), Point(0,0,0),
                                 Point(-1000, -200, -1), Point(1000, 1000, 1) ) )
                                 
  
  
  return world;

def makeLaneNew( xStart, xEnd, yStart, yEnd, nLanes, horizontal ):
  global cityMod
  if ( horizontal ):
    cityMod.addMaterial( CityGridModulator.HROAD,
                         Point2D( xStart, yStart ),
                         Point2D( xEnd, yEnd ) )
  else:
    cityMod.addMaterial( CityGridModulator.VROAD,
                         Point2D( xStart, yStart ),
                         Point2D( xEnd, yEnd ) )
  

def createCityBuildingsNew( world ):
    group = GriddedGroup(5);
    start = D2(-230, -20);
    end = D2( 230, 220 );
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

        makeLaneNew( current.x,
                     current.x + blkSize.x,
                     current.y + blkSize.y,
                     current.y + blkSize.y + yStreetLanes[y] * laneSize,
                     yStreetLanes[y],
                     True )
        current.y += blkSize.y + yStreetLanes[y] * laneSize;
        
      makeLaneNew( current.x + blkSize.x, current.x + blkSize.x + xStreetLanes[x] * laneSize, start.y,  end.y, xStreetLanes[x], False )
      current.x += blkSize.x + xStreetLanes[x] * laneSize;

    nShoreLanes = int(3 * drand48()) + 1
    makeLaneNew( start.x, end.x,
                 start.y - nShoreLanes*laneSize, start.y,
                 nShoreLanes,
                 True)
    world.addObject(group);


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
    start = D2(-230, -20);
    end = D2( 230, 220 );
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
scene.setBackground( GradientBackground( Color (1, 0, 0 ),
                                         Color (0,1,1),
                                         False, 3.0 ) );
if ( False ):
  world.addObject( makeGround() );
  createCityBuildings( world );
else:
  world.addObject(makeGroundNew())
  createCityBuildingsNew( world )

scene.setObject( world );

scene.addLight(PointLight(Point(20, -30, 30), Color(.2,.2,.2)));
scene.addLight(DirectionalLight( Vector(0.426, -0.639, 0.639),
                                 Color( 1, 0.9, 0.7) ) )

scene.setCamera(PinholeCamera(Point(0, -50, 22.5),
                              Point(0, 0, 0),
                              Vector(0, 0, 1),
                              55));
scene.setMaxRayDepth(10);
scene.setMinAttenuation(.01);
