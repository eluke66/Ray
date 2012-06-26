from Core import *
from Background import *
from Materials import *
from Geometry import *
from IO import *

def makeGround():
  world = Group();
  g = lambertianMaterial( Color(0.5, 0.5, 0.5), 0.6, 0.4 );
  world.addObject( Plane( g, Vector(0,0,1), Point(0,0,0) ) );
  hfmatl = phongImageMaterial( "textures/waterblue.ppm",
                               0.6, 0.4,
                               Color(0,0,1),
                               100 );
  nTilesY = 1;
  nTilesX = 2;
  p = Point( -64.0, -20.0, 0 );
  tileSizeX = 128.0 / float(nTilesX);
  tileSizeY = 20 / float(nTilesY);

  h = Heightfield(hfmatl,
                  "waves2k.hf",
                  p,
                  p+Vector(tileSizeX,tileSizeY,0) );
  fudge = 0.01;
  waterMin = h.getMinimum();

  for x in range(0,nTilesX):
      for y in range(0,nTilesY):
          pos = Position();
          pos.translate( Vector(x*(tileSizeX-fudge), y*(tileSizeY-fudge), -waterMin ), False );
          i = Instance( pos.getMatrix( h ),
                        h,
                        hfmatl );
          world.addObject(i);

  return world;

import os
def verifyFile(name):
    try:
        os.stat(name)
    except OSError:
        return False
    return True


                 
buildings = []
def isModel(filename):
  buildingNames = ["atrium_mrealms", "Bldng106", "DTowerV1packed",
                 "office2_mrealms", "office3_mrealms",
                 "office_sp_fournel", "OfficeTowerV1",
                 "skyscraper2_mrealms", "skyscraper4_mrealms" ]
  for b in buildingNames:
    if os.path.basename(filename).startswith(b):
      return True
  return False
  #fn = filename.lower()
  #return fn.endswith("3ds")

def loadBuilding(buildings, dirname, fnames):
  for modelFile in fnames:
    filename = dirname + "/" + modelFile
    if (verifyFile( filename ) and isModel(filename) ):
      obj = ObjectLoader.LoadObject( filename, "" )
      if ( obj is not None ):
        buildings.append( obj )
        print filename
  
import os.path
def loadBuildingModels():
  global buildings
  if ( len(buildings) > 0 ):
    return buildings
  
  path = "/home/luke/Skyscrapers";


  os.path.walk( path, loadBuilding, buildings )
  print "We successfully loaded",len(buildings),"buildings"
  return buildings


squat= None;
talls = []

def fillSquareWithModel( xStart, xEnd, yStart, yEnd ):
    models = loadBuildingModels()
    
    height = 3.0 + 18*drand48();
    obj = models[ random.randrange(0,len(models) ) ]
    sf = Position.ScaleToFit( obj,
                              BoundingBox( Point( xStart, yStart, 0 ),
                                           Point( xEnd,   yEnd,   height ) ))
                              
    p = Position()
    p.translate( Vector( xStart, yStart, 0 ), True )
    p.scale( sf, Position.SCALE_REL )
    m = p.getMatrix(obj)
    return Instance( m, obj )

import random
def fillSquareWithBuildings( xStart, xEnd, yStart, yEnd ):
    global squat, talls
    
    height = 3.0 + 18*drand48();
    width = (xEnd - xStart);

    if ( len(talls) == 0 ):
        i = 0
        while (True):
          ostr = "textures/tallBuilding" + str(i) + ".jpg"
          if ( verifyFile( ostr ) ):
            mapStr = "textures/tbMap" + str(i) + ".jpg"
            uRepeat = 1.0
            vRepeat = 2.0
            tc = TextureColor( ostr, uRepeat, vRepeat )
            if ( verifyFile( mapStr ) ):
              shinyMat = repeatingPhongImageMaterialTC( tc, uRepeat, vRepeat, 0.6, 0.4, Color(0,0,0), 80)
              dullMat  = repeatingLambertianImageMaterialTC( tc, 0.6, 0.4 )
              modulator = TextureModulator( ImageLoader.LoadImage( mapStr ), uRepeat, vRepeat )
              talls.append( MaterialModulator( modulator, [dullMat, shinyMat] ) )
            else:
              talls.append( repeatingPhongImageMaterialTC( tc, 1.0, 2.0, 0.6, 0.4, Color(0,0,0), 9990) );
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
        r = repeatingPhongImageMaterialTC(hRoad,
                                          (xEnd - xStart) / laneSize, float(nLanes),
                                          0.6, 0.4, Color(0,0,0), 1 );
    
    else:
        if ( vRoad is None ):
            vRoad = TextureColor( "textures/Road.jpg" );
        r = repeatingPhongImageMaterialTC(vRoad,
                                          float(nLanes), (yEnd - yStart) / laneSize,
                                          0.6, 0.4, Color(0,0,0), 1 );
        
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
    start = D2(-130, 10);
    end = D2( 130, 250 );
    avgBlocksize = D2( 5, 5 );
    blockVariance = D2( 2, 2 );
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
        blkSize.y = yBlockSizes[y]
        if ( y == 0 ):
          group.addObject( fillSquareWithModel( current.x, current.x + blkSize.x,
                                                current.y, current.y + blkSize.y ) )
        else:
          group.addObject( fillSquareWithBuildings( current.x, current.x + blkSize.x,
                                                    current.y, current.y + blkSize.y ) );

        
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

    world.addObject(group);


import os
scene = Scene()
world = Group();
scene.setBackground( GradientBackground( Color (.745, 0.462, 0.313 ),
                                         Color (0,0,1),
                                         False, 3.0 ) );
world.addObject( makeGround() );

# TEMP YUCK
models = loadBuildingModels()
q = Group()
for m in models:
  q.addObject(m)
ppp = Position()
ppp.translate( Vector(0, 0, -1000 ), True )
ppp.transform(q)
world.addObject(q)
# EOYUCK

createCityBuildings( world );

scene.setObject( world );
scene.setAmbient(Color(.2, .2, .2));

scene.addLight(PointLight(Point(20, -30, 30), Color(.9,.9,.9)));
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
