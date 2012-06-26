#include "all.h"
#include "Background/GradientBackground.h"
#include "Core/Position.h"
#include "Geometry/Instance.h"
#include <sstream>

Object *makeGround() {
  Group * world = new Group();
  Material * g = new LambertianMaterial( Color(0.5, 0.5, 0.5), 0.6, 0.4 );
  world->addObject( new Plane( g, Vector(0,0,1), Point(0,0,0) ) );
#if 1
  
  Material *hfmatl = new PhongImageMaterial( "textures/waterblue.ppm",
					     0.6, 0.4,
					     Color(0,0,1),
					     100 );
  int nTilesY = 1;
  int nTilesX = 2;
  Point p( -64.0, -20.0, 0 );
  double tileSizeX = 128.0 / (double)nTilesX;
  double tileSizeY = 20 / (double)nTilesY;

  Heightfield * h = new Heightfield(hfmatl,
				    "waves2k.hf",
				    p,
				    p+Vector(tileSizeX,tileSizeY,0) );
  double fudge = 0.01;
  float waterMin = h->getMinimum();
  
  for ( int x = 0; x < nTilesX; ++x ) {
    for ( int y = 0; y < nTilesY; ++y ) {
      Position pos;
      pos.translate( Vector(x*(tileSizeX-fudge), y*(tileSizeY-fudge), -waterMin ), false );
      Instance *i = new Instance( pos.getMatrix( h ),
				  h,
				  hfmatl );
      world->addObject(i);
    }
  }
#endif
  return world;
}

Material *squat=NULL;
std::vector<Material *> talls;

Object *
fillSquareWithBuildings( float xStart, float xEnd, float yStart, float yEnd ) {

  double height = 3.0 + 18*drand48();
  double width = (xEnd - xStart);

  if ( talls.size() == 0 ) {
    for (unsigned i = 0; ; ++i ) {
      std::ostringstream ostr;
      ostr << "textures/tallBuilding" << i << ".jpg";
      if ( verifyFile( ostr.str() ) )
	talls.push_back( new RepeatingPhongImageMaterial( ostr.str(), 1.0, 2.0, 0.6, 0.4, Color(0,0,0), 80) );
      else
	break;
    }
    std::cerr << "Found " << talls.size() << " building textures. " << std::endl;
  }
  
  if ( squat == NULL )
    squat = new PhongImageMaterial("textures/shortBuilding.jpg", 0.6, 0.4, Color(0,0,0), 80);

  Material *y;
  if ( height > width ) {
    unsigned idx = (unsigned)(drand48() * talls.size());
    if ( idx == talls.size() )
      idx--;
    y = talls[ idx ];
  }
  else
    y = squat;

#if 1
  return new Box( y,
		  Point( xStart, yStart, 0 ),
		  Point( xEnd,   yEnd,   height  ) );
#else
  return new Box( NULL, Point(0,0,0), Point(0,0,0) );
#endif
}

TextureColor *vRoad=NULL, *hRoad=NULL;

Object * makeLane( float xStart, float xEnd, float yStart, float yEnd, int nLanes, bool horizontal ) {
  float laneSize = 2.0f;
  Material * r = NULL;
  if ( horizontal ) {
    if ( !hRoad ) 
      hRoad = new TextureColor( "textures/Roadh.jpg" );
    r = new RepeatingPhongImageMaterial(*hRoad,
					(xEnd - xStart) / laneSize, (double)nLanes,
					0.6, 0.4, Color(0,0,0), 1 );
  }
  else {
    if ( !vRoad ) 
      vRoad = new TextureColor( "textures/Road.jpg" );
    r = new RepeatingPhongImageMaterial(*vRoad,
					(double)nLanes, (yEnd - yStart) / laneSize,
					0.6, 0.4, Color(0,0,0), 1 );
  }
  return new Box( r,
		  Point( xStart, yStart, 0 ),
		  Point( xEnd,   yEnd,   1e-4 ) );
}


struct D2 {
  D2():x(0),y(0) {}
  D2(float x, float y) : x(x), y(y) {}
  float x,y;
};

int getLanes() {
  double s = drand48();
  int numLanes = 4;
  if ( s <= 0.5 )
    numLanes = 1;
  else if ( s <= 0.75 )
    numLanes = 2;
  else if ( s <= 0.9 )
    numLanes = 3;
  return numLanes;
}

void createCityBuildings( Group *world ) {
  GriddedGroup *group = new GriddedGroup(5);
  D2 start(-130, 10);
  D2 end( 130, 250 );
  D2 avgBlocksize( 5, 5 );
  D2 blockVariance( 2, 2 );
  float laneSize = 2.0f;
  D2 current;
  
  std::vector<float> xBlockSizes;
  std::vector<float> yBlockSizes;
  std::vector<int>   xStreetLanes;
  std::vector<int>   yStreetLanes;
  
  current.x = start.x;
  while ( current.x < end.x ) {
    float blkSize = avgBlocksize.x +
      blockVariance.x * (drand48() - 0.5);
    xBlockSizes.push_back( blkSize );
    int numLanes = getLanes();
    xStreetLanes.push_back( numLanes );
    current.x += blkSize + numLanes * laneSize;
  }
  
  current.y = start.y;
  while ( current.y < end.y ) {
    float blkSize = avgBlocksize.y +
      blockVariance.y * (drand48() - 0.5);
    yBlockSizes.push_back( blkSize );
    int numLanes = getLanes();
    yStreetLanes.push_back( numLanes );
    current.y += blkSize + numLanes * laneSize;
  }
  
  current.x = start.x;
  D2  blkSize;
  for ( unsigned x = 0; x < xBlockSizes.size(); ++x  ) {
    current.y = start.y;
    blkSize.x = xBlockSizes[x];
    for ( unsigned y = 0; y < yBlockSizes.size(); ++y ) {
      blkSize.y = yBlockSizes[y];
      group->addObject( fillSquareWithBuildings( current.x, current.x + blkSize.x,
						 current.y, current.y + blkSize.y ) );

      group->addObject( makeLane( current.x,
				  current.x + blkSize.x,
				  current.y + blkSize.y,
				  current.y + blkSize.y + yStreetLanes[y] * laneSize,
				  yStreetLanes[y],
				  true ) );
      current.y += blkSize.y + yStreetLanes[y] * laneSize;
    }
    group->addObject( makeLane( current.x + blkSize.x,
				current.x + blkSize.x + xStreetLanes[x] * laneSize,
				start.y,
				end.y,
				xStreetLanes[x],
				false ) );
    current.x += blkSize.x + xStreetLanes[x] * laneSize;
  }

  // Add the final collection to the world.
  world->addObject(group);
}


void createCityBuildings1( Group * world ) {

  unsigned numXBlocks = 50;
  unsigned numYBlocks = 50;
  float xStart = -130;
  float xEnd   = 130;
  float yStart = 10;
  float yEnd   = 250;
  float streetWidth = 2;
  GriddedGroup * group = new GriddedGroup( 5 );
  
  float xBlockSize = (xEnd - xStart - streetWidth) / numXBlocks - streetWidth;
  float yBlockSize = (yEnd - yStart - streetWidth) / numYBlocks - streetWidth;
  
  float currentX = xStart;
  for ( unsigned x = 0; x < numXBlocks; ++x, currentX += xBlockSize+streetWidth ) {
    float currentY = yStart;
    for ( unsigned y = 0; y < numYBlocks; ++y, currentY += yBlockSize+streetWidth ) {
      group->addObject( fillSquareWithBuildings( currentX,
						 currentX + xBlockSize,
						 currentY,
						 currentY + yBlockSize ) );
    }
  }

  world->addObject(group);
}


Scene * city()
{
  Scene* scene = new Scene();
  Group* world = new Group();
  //  scene->setBackground(new ConstantBackground(Color(0.2, 0.5, 1)));
  scene->setBackground( new GradientBackground( Color (.745, 0.462, 0.313 ),
						Color (0,0,1),
						false, 3.0 ) );
  world->addObject( makeGround() );

  createCityBuildings( world );

  scene->setObject( world );
  scene->setAmbient(Color(.2, .2, .2));

  scene->addLight(new PointLight(Point(20, -30, 30), Color(.9,.9,.9)));
  if ( getenv( "MOVE_CAMERA") ) {
    scene->setCamera(new PinholeCamera(Point(0 + 40*(drand48()-0.5), -40, 7.5 + 5*drand48()),
				       Point(0, 0, 0),
				       Vector(0, 0, 1),
				       55));
  }
  else {
    scene->setCamera(new PinholeCamera(Point(0, -60, 22.5),
				       Point(0, 0, 0),
				       Vector(0, 0, 1),
				       55));
  }
  scene->setMaxRayDepth(10);
  scene->setMinAttenuation(.01);
  
  return scene;

}


ADD_SCENE(city);
