from Core import *
from Background import *
from Materials import *
from Geometry import *
from IO import *
from Filter import *

import scenes


def fillSquareWithBuildings1( xStart, xEnd, yStart, yEnd ):
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
    height = 30.0 + 30*drand48()
  elif ( bType > 0.75 ):
    height = 30.0 + 80*drand48()
  elif ( bType > 0.87 ):
    height = 30.0 + 160*drand48()
  elif ( bType > 0.94 ):
    height = 30.0 + 320*drand48()
  else:
    height = 30.0 + 640*drand48()
  
  
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


  bGroup = Group()
  minInset = 0.5
  insetSize = 2.0

  x1 = insetSize*drand48() + minInset
  x2 = insetSize*drand48() + minInset
  y1 = insetSize*drand48() + minInset
  y2 = insetSize*drand48() + minInset
  bGroup.addObject(Building( buildingMat,
                             nonWinMat,
                             Point( xStart+x1, yStart+y1, 0 ),
                             Point( xEnd-x2,   yEnd-y2,   height  ) ))
  # Add sidewalks
  swWidth = 0.5
  swMatl = lambertianMaterial( Color(1,1,1), 1, 0.4 )
  bGroup.addObject(Box( swMatl,
                        Point(xStart, yStart, 0),
                        Point(xEnd, yStart+swWidth, 0.1 ) ) )
  bGroup.addObject(Box( swMatl,
                        Point(xStart, yEnd-swWidth, 0),
                        Point(xEnd, yEnd, 0.1 ) ) )
  bGroup.addObject(Box( swMatl,
                        Point(xStart, yStart+swWidth, 0),
                        Point(xStart+swWidth, yEnd-swWidth, 0.1 ) ) )
  bGroup.addObject(Box( swMatl,
                        Point(xEnd-swWidth, yStart+swWidth, 0),
                        Point(xEnd, yEnd-swWidth, 0.1 ) ) )
  return bGroup

vRoad=None
hRoad=None

def makeLane( xStart, xEnd, yStart, yEnd, nLanes, horizontal ):
    global hRoad, vRoad
    
    laneSize = 6.0;
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

intersection=None
def makeIntersection( xStart, xEnd, yStart, yEnd ):
  global intersection

  if ( intersection is None ):
    intersection = TextureColor( "textures/Roadi.jpg" )
  r = repeatingLambertianImageMaterialTC( intersection, 1,1, 0.6, 0.4 )
  return Box( r, 
              Point( xStart, yStart, 0 ),
              Point( xEnd,   yEnd,   1e-4 ) )                   


class D2:
    def __init__(self,x=0,y=0):
        self.x = x
        self.y = y
        

def getLanes():
  s = drand48()
  numLanes = 5
  if ( s <= 0.5 ):
    numLanes = 1
  elif ( s <= 0.75 ):
    numLanes = 2
  elif ( s <= 0.9 ):
    numLanes = 3
  elif ( s <= 0.95 ):
    numLanes  = 4
  return numLanes;


import time
def createCityBuildings( world, xmin, xmax, ymin, ymax, groundProfile ):
    group = GriddedGroup(10);

    scenes.setCityParams( Point2D( (xmin+xmax)/2.0,(ymin+ymax)/2.0),
                          Point2D( xmax-xmin, ymax - ymin ) )
                                   
    start = D2(xmin, ymin);
    end = D2( xmax, ymax );
    avgBlocksize = D2( 100, 100 );
    blockVariance = D2( 50, 50 );
    laneSize = 8.0;
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

    print "We will have",len(xBlockSizes),"blocks in X"
  
    current.y = start.y;
    while ( current.y < end.y ):
        blkSize = avgBlocksize.y + blockVariance.y * (drand48() - 0.5);
        yBlockSizes.append( blkSize );
        numLanes = getLanes();
        yStreetLanes.append( numLanes );
        current.y += blkSize + numLanes * laneSize;
      
    print "We will have",len(yBlockSizes),"blocks in Y"
    
    current.x = start.x;
    blkSize = D2()

    for x in range(0,len(xBlockSizes)):
      current.y = start.y;
      blkSize.x = xBlockSizes[x];
      for y in range(0,len(yBlockSizes)):
        
        blkSize.y = yBlockSizes[y];
        scenes.makeSquareNew(group,
                          current.x, current.y,
                          blkSize.x, blkSize.y,
                          laneSize,
                          yStreetLanes[y],
                          xStreetLanes[x],
                          groundProfile)
        current.y += blkSize.y + yStreetLanes[y] * laneSize;
        
      current.x += blkSize.x + xStreetLanes[x] * laneSize;

    scenes.printAccumTime()
    nShoreLanes = int(3 * drand48()) + 1
    group.addObject( makeLane( start.x, end.x,
                               start.y - nShoreLanes*laneSize, start.y,
                               nShoreLanes,
                               True) )
    scenes.incarnateBridges(group);
    world.addObject(group);
