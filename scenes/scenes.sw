%module scenes
%{
#include "Geometry/Bridge.h"
#include "Geometry/GriddedGroup.h"
#include "Core/Color.h"
#include "Core/Time.h"
#include "Core/Scene.h"
#include "Core/Position.h"
#include "Geometry/Primitive.h"
#include "Geometry/Box.h"
#include "Geometry/BoundedPlane.h"
#include "Geometry/Building.h"
#include "Geometry/Mountains.h"
#include "Geometry/Instance.h"
#include "Geometry/Water.h"
#include "Material/TextureColor.h"
#include "Material/MetalMaterial.h"
#include "Material/LambertianMaterial.h"
#include "Material/CheckerMaterial.h"
#include "Material/Modulators.h"
#include "Material/MaterialModulator.h"
#include "Material/BasicShade.h"
#include "Core/utils.h"
#include "IO/ObjectLoader.h"
#include <vector>
#include <list>
  

#define RepeatingLambertianImageMaterialTC(a,a1,a2,b,c) \
  _Material<TextureColor,LambertianLighting>( TextureColor(*a,a1,a2), LambertianLighting(b,c))


  // Safe building height above water level...
  const double WATER_LEVEL = 2.0;

  // Skyscraper decay constant
  const double SKY_CONSTANT = 1500000.; // WAS 300000.;

  // Midsize decay constant
  const double MID_CONSTANT = 1000000.0;// WAS 1000000.;

  const double MAX_SKYSCRAPER_HEIGHT = 700.0;
  const double SIDEWALK_WIDTH = 10.0;
  const double ROAD_LANE_WIDTH = 6.0;
  const double MAX_BRIDGE_HEIGHT = 200.0;
  const double BRIDGE_HEIGHT_FACTOR = 0.15;
  const double SUSPENSION_CABLE_SPACING = 25.0;
  const double SUSPENSION_CABLE_THICKNESS = 1.5;
  const double SUSPENSION_CABLE_SUPPORT_THICKNESS = 0.65;
  const int MIN_BRIDGE_LANES = 4;
  const double SQUARES_WITH_BUILDINGS = 0.9;
  const double PLATFORM_BRIDGE_PERCENT = 0.55;
  const double SUSPENSION_BRIDGE_PERCENT = PLATFORM_BRIDGE_PERCENT + 0.35;
  
  // Water line
  const int UNDERWATER_TESTS = 10;
  const int NUM_WATERLINE_SQUARES = 50;

  // Trees
  const double MAX_TREE_HEIGHT = 20.0;
  const double TREE_SPACING = 10.0;
  const double TREE_DENSITY = 0.5;
  
  static Point2D CityCenter;
  static Point2D CitySize;
  static bool CityParamsSet = false;
  
  void setCityParams( const Point2D& center,
		      const Point2D& size )
  {
    CityCenter = center;
    CitySize = size;
    CityParamsSet = true;
  }
  
  BuildingFactory::BuildingType getBuildingType( double x, double y ) {
    double dist =
      (x-CityCenter.x())*(x-CityCenter.x()) +
      (y-CityCenter.y())*(y-CityCenter.y());
    
    // First test to see if it's a skyscraper.
    double skyProb = exp( -dist / SKY_CONSTANT );
    if ( drand48() < skyProb )
      return BuildingFactory::SKYSCRAPER;
    
    // Then a midsize.
    double midProb = exp( -dist / MID_CONSTANT );
    if ( drand48() < midProb )
      return BuildingFactory::MIDSIZE;
    
    // Otherwise, it's a brick.
    return BuildingFactory::BRICK_BUILDING;
    
  }
  
  void fillSquareWithBuildingsC( Group *group,
				 Point min, Point max ) {
    static bool init = false;
    if ( !init ) {
      BuildingFactory::Initialize( 1, MAX_SKYSCRAPER_HEIGHT );
      init = true;
    }

    // Sidewalks are 10 feet wide.
    double swWidth = SIDEWALK_WIDTH;
    double xmin = min.x(), xmax=max.x();
    double ymin = min.y(), ymax=max.y();
    static Material * swMatl = new LambertianMaterial( Color(1,1,1), 1, 0.4 );
    group->addObject(new Box( swMatl,
			      Point(xmin, ymin, 0),
			      Point(xmax, ymin+swWidth, 0.1 ) ) );
    group->addObject(new Box( swMatl,
			  Point(xmin, ymax-swWidth, 0),
			  Point(xmax, ymax, 0.1 ) ) );
    group->addObject(new Box( swMatl,
			  Point(xmin, ymin+swWidth, 0),
			  Point(xmin+swWidth, ymax-swWidth, 0.1 ) ) );
    group->addObject(new Box( swMatl,
			  Point(xmax-swWidth, ymin+swWidth, 0),
			  Point(xmax, ymax-swWidth, 0.1 ) ) );
    
    BuildingFactory::BuildingType type = getBuildingType( (xmax+xmin)/2.0,
							  (ymax+ymin)/2.0);

    group->addObject( BuildingFactory::CreateBuilding( type,
						       min,
						       max,
						       0 ) );

  }

  //static TextureColor *hRoad=NULL;
  static TextureColor *hRoad=new TextureColor( "textures/Roadh.jpg" );
  static TextureColor *vRoad=new TextureColor( "textures/Road.jpg" );;

  Primitive * makeLane( float xStart, float xEnd,
			float yStart, float yEnd,
			int nLanes, bool horizontal,
			double elevation = 0.0) {
    float laneSize = ROAD_LANE_WIDTH;
    Material *r = NULL;
    
    if ( horizontal ) {
      if ( hRoad == NULL )
	hRoad = new TextureColor( "textures/Roadh.jpg" );
      r = new RepeatingLambertianImageMaterialTC(hRoad,
						 (xEnd - xStart) / laneSize,
						 float(nLanes),
						 0.6, 0.4);
    }
    else {
      if ( vRoad == NULL )
	vRoad = new TextureColor( "textures/Road.jpg" );
      r = new RepeatingLambertianImageMaterialTC(vRoad,
						 float(nLanes), (yEnd - yStart) / laneSize,
						 0.6, 0.4 );
    }
    
    return new Box( r,
		    Point( xStart, yStart, elevation ),
		    Point( xEnd,   yEnd,   elevation + 1e-4 ) );
  }
 
  void spreadWater( Group *group,
		    Water *water,
		    Point min, Point max,
		    int xRes, int yRes,
		    double extraOffset )
  {
    double tileSizeX = (max.x() - min.x()) / double(xRes);
    double tileSizeY = (max.y() - min.y()) / double(yRes);
    
    double fudge = 0.01;
    double waterMin = water->getMinimum();

    for ( int x = 0; x < xRes; ++x )
      for ( int y = 0; y < yRes; ++y ) {
	Position pos;
	pos.translate( Vector(x*(tileSizeX-fudge), y*(tileSizeY-fudge), -waterMin+extraOffset ), false );
	if ( x != 0 && y != 0 ) 
	  group->addObject(new Instance( pos.getMatrix( water ),
					 water,
					 water->matl ) );
      }
    group->addObject(water);
  }


  static CityGridModulator * cityMod = NULL;
  
  void makeLaneNew( const Point2D &min, const Point2D &max,
		    bool horizontal )
  {
    assert( cityMod );
    cityMod->addMaterial( horizontal ? CityGridModulator::HROAD : CityGridModulator::VROAD,
			  min, max );
  }
  
  void makeIntersectionNew( const Point2D &min, const Point2D &max )
  {
    assert( cityMod );
    cityMod->addMaterial( CityGridModulator::INTERSECTION,
			  min, max );
  }


  void loadCityMod(Group *group, Mountains *mtns)
  {
    assert( CityParamsSet );
    
    MaterialList mats;
    if ( mtns )
      mats.push_back( mtns->getMaterial() );
    else
      mats.push_back( new LambertianMaterial( Color(0,0.8,0), 0.5, 0.4 ) );
    mats.push_back( new RepeatingLambertianImageMaterialTC(new TextureColor("textures/Roadh.jpg"),
							   1,1 ,
							   0.6, 0.4) );
    mats.push_back( new RepeatingLambertianImageMaterialTC(new TextureColor("textures/Road.jpg"),
							   1,1 ,
							   0.6, 0.4) );

    mats.push_back( new RepeatingLambertianImageMaterialTC(new TextureColor("textures/Roadi.jpg"),
							   1,1 ,
							   0.6, 0.4) );

    cityMod = new CityGridModulator( (Point2D)(CityCenter - (CitySize * 0.5)),
				     CityCenter + (CitySize * 0.5),
				     10 );

    MaterialModulator *mmod = new MaterialModulator( cityMod, mats );
    if ( mtns ) {
      mtns->setMaterial( mmod,true );
    }
    else {
      group->addObject( new BoundedPlane( mmod, Vector(0,0,1), Point(0,0,0),
					  Point( CityCenter.x() - CitySize.x() * 0.5,
						 CityCenter.y() - CitySize.y() * 0.5,
						 -1 ),
					  Point( CityCenter.x() + CitySize.x() * 0.5,
						 CityCenter.y() + CitySize.y() * 0.5,
						 1 ) ) );
    }
  }

  bool underwater( Mountains *mtns,
		   const Point2D &min,
		   const Point2D &max,
		   double *minLoc = NULL) {
    if (!mtns)
      return false;

    if ( minLoc ) {
      *minLoc = std::min( *minLoc, mtns->valueAt( min.x(), min.y() ) );
      if ( *minLoc <= WATER_LEVEL ) return true;
      *minLoc = std::min( *minLoc, mtns->valueAt( min.x(), max.y() ) );
      if ( *minLoc <= WATER_LEVEL ) return true;
      *minLoc = std::min( *minLoc, mtns->valueAt( max.x(), min.y() ) );
      if ( *minLoc <= WATER_LEVEL ) return true;
      *minLoc = std::min( *minLoc, mtns->valueAt( max.x(), max.y() ) );
      return  *minLoc <= WATER_LEVEL;
      
    }
    else {
      double x, y;
      
      for ( int i = 0; i < UNDERWATER_TESTS; ++i ) {
	x = min.x() + drand48() * (max.x() - min.x());
	y = min.y() + drand48() * (max.y() - min.y());
	if ( mtns->valueAt( x, y ) <= WATER_LEVEL )
	  return true;
      }
      return
	(mtns->valueAt( min.x(), min.y() ) <= WATER_LEVEL) ||
	(mtns->valueAt( min.x(), max.y() ) <= WATER_LEVEL) ||
	(mtns->valueAt( max.x(), min.y() ) <= WATER_LEVEL) ||
	(mtns->valueAt( max.x(), max.y() ) <= WATER_LEVEL);
    }
  }

  struct BridgeProxy {
  public:
    BridgeProxy( const Point &start, const Point &end,
		 int numLanes, bool vertical ) :
      _start(start), _end(end), _numLanes(numLanes), _vertical(vertical),
      _collisions(0) {}

    bool intersects( const BridgeProxy &p ) const {
      if ( _vertical ) {
	if ( p._vertical ) return false;

	return
	  (_start.x() >= p._start.x() ||
	   _end.x() <= p._end.x()) &&
	  (p._start.y() >= _start.y() ||
	   p._end.y() <= _end.y());
      }
      else {
	if ( !p._vertical ) return false;

	return p.intersects(*this);
      }
    }
    Point _start;
    Point _end;
    int _numLanes;
    bool _vertical;

    unsigned _collisions;
  };
  typedef std::list<BridgeProxy> BridgeList;
  typedef BridgeList::iterator BridgeListIterator;
  static BridgeList bridges;
  struct BridgeCollision {
    BridgeCollision(BridgeListIterator p1, BridgeListIterator p2) :
      p1(p1), p2(p2) {}
    bool references( BridgeListIterator a ) const {
      return a == p1 || a == p2;
    }
    BridgeListIterator p1;
    BridgeListIterator p2;
  };
  typedef std::list<BridgeCollision> BridgeCollisionList;
  static BridgeCollisionList bridgeCollisions;
  
  void incarnateBridges( Group * group ) {
    //std::cerr << "Initially we have  " << bridges.size() << " bridges" << std::endl;
    
    // Loop over all bridges, comparing each against all others.
    // If there is a collision, note it.
    for ( BridgeList::iterator it = bridges.begin();
	  it != bridges.end();
	  ++it )
      for ( BridgeList::iterator jt = bridges.begin();
	  jt != bridges.end();
	  ++jt )
	if ( it->intersects( *jt ) ) {
	  //std::cerr << "Found collision: " << std::endl;
	  //std::cerr << "\t" << it->_start << " -> " << it->_end << " " << (it->_vertical ? "vertical" : "horizontal" ) << std::endl;
	  //std::cerr << "\t" << jt->_start << " -> " << jt->_end << " " << (jt->_vertical ? "vertical" : "horizontal" ) << std::endl;
	  bridgeCollisions.push_back( BridgeCollision(it, jt) );
	  it->_collisions++;
	  jt->_collisions++;
	}

    // Until there are no collisions left:
    while ( !bridgeCollisions.empty() ) {
      
      // Remove the bridge with the most collisions, and remove each
      // collision item that references that bridge.
      BridgeList::iterator whichBridge = bridges.end();
      unsigned numCollisions = 0;
      for ( BridgeList::iterator it = bridges.begin();
	    it != bridges.end();
	    ++it ) {
	if ( it->_collisions > numCollisions ) {
	  whichBridge = it;
	  numCollisions = it->_collisions;
	}
      }

      //std::cerr << "Bridge " << whichBridge->_start << " -> " << whichBridge->_end
      //		<< " has " << numCollisions << " collisions!" << std::endl;
      //std::cerr << "\tCollisions before removing: " << bridgeCollisions.size();
      for (BridgeCollisionList::iterator bc = bridgeCollisions.begin();
	   bc != bridgeCollisions.end(); ) {
	if ( bc->references( whichBridge ) ) {
	  BridgeCollisionList::iterator tmp = bc;
	  ++bc;
	  bridgeCollisions.erase(tmp);
	}
	else {
	  ++bc;
	}
      }
      //std::cerr << " After removing: " << bridgeCollisions.size() << std::endl;
      bridges.erase(whichBridge);

    }

    // Loop over all bridges, adding them to the group.
    //std::cerr << "We should create  " << bridges.size() << " bridges" << std::endl;
    for ( BridgeList::iterator it = bridges.begin();
	  it != bridges.end();
	  ++it ) {

      double bridgeType = drand48();
      Material *r = NULL;
      float laneSize = ROAD_LANE_WIDTH;
      Point start(it->_start);
      Point end(it->_end);
      int numLanes = it->_numLanes;
      bool vertical = it->_vertical;
      
      if ( !vertical ) {
	assert(hRoad);
	r = new RepeatingLambertianImageMaterialTC(hRoad,
						   (end.x() - start.x()) / laneSize,
						   float(numLanes),
						   0.6, 0.4);
      }
      else {
	assert(vRoad);
	r = new RepeatingLambertianImageMaterialTC(vRoad,
						   float(numLanes), (end.y() - start.y()) / laneSize,
						   0.6, 0.4 );
      }
      
      
      double height = Vector(end-start).length() * BRIDGE_HEIGHT_FACTOR;
      if ( height > MAX_BRIDGE_HEIGHT ) {
	height = MAX_BRIDGE_HEIGHT;
      }
    
      if ( bridgeType < PLATFORM_BRIDGE_PERCENT )
	{
	  group->addObject( new PlatformBridge( r, start, end, height, WATER_LEVEL ) );
	}
      
      else if ( bridgeType < SUSPENSION_BRIDGE_PERCENT )
	{
	  int numTowers = int((Vector(end-start).length() * BRIDGE_HEIGHT_FACTOR) / height) + 1;
	  SuspensionBridge * bridge = new SuspensionBridge( r, start, end,
							    height,
							    WATER_LEVEL,
							    SUSPENSION_CABLE_SPACING,
							    numTowers );
	  bridge->setMainCableThickness(SUSPENSION_CABLE_THICKNESS);
	  bridge->setVerticalCableThickness(SUSPENSION_CABLE_SUPPORT_THICKNESS);
	  bridge->setTowerThickness(3.0);
	  group->addObject( bridge );
	}
      else {
	group->addObject( new ArchBridge( r, start, end, height, WATER_LEVEL ) );
      }
    }

    
  }
  
  void makeBridge(Group * group,
		  const Point &start, const Point &end,
		  int numLanes, bool vertical)
  {
    bridges.push_back( BridgeProxy(start,end,numLanes,vertical) );
  }

  void setupWaterLine(Group *group, Mountains * mtns,
		      const Point2D &start, const Point2D &end ) {
    if ( ! mtns ) return;

    int theSign = sign(mtns->valueAt( start.x(), start.y() ));
    if ( sign(mtns->valueAt( end.x(), start.y() )) == theSign )
      if ( sign(mtns->valueAt( end.x(), end.y() )) == theSign )
	if ( sign(mtns->valueAt( start.x(), end.y() )) == theSign )
	  if ( sign(mtns->valueAt( (start.x() + end.x())*0.5, (start.y()+ end.y())*0.5)) == theSign )
	    return;
    static Material * concrete = BuildingFactory::Concrete(0.2);
    Vector2D d(end-start);

    Vector2D nSquares(NUM_WATERLINE_SQUARES,
		      NUM_WATERLINE_SQUARES);
    Vector2D diff( d.x() / nSquares.x(),
		   d.y() / nSquares.y() );

    for ( int x = 0; x < nSquares.x(); ++x ) {
      for (int y = 0; y < nSquares.y(); ++y ) {
	Point2D location( start.x() + x*diff.x(),
			  start.y() + y*diff.y() );
	
	// If the spot is within 6 inches of 0, add a box
	if ( ABSCLOSE( mtns->valueAt( location.x(), location.y() ),
		       0,
		       0.5 ) ) {


	  
	  group->addObject( new Box( concrete,
				     Point( location.x(), location.y(),  - 1 ),
				     Point( location.x() + diff.x(),
					    location.y() + diff.y(),
					    4 ) ) );
	}
      }
    }
    
  }
	       
  static std::vector<Object *> trees;
  static std::vector<BoundingBox> tboxes;
  static bool loadedTrees = false;
  
  void loadTrees()
  {
    // Load tree models.
    if ( !loadedTrees ) {
      // This could go up to 9!
      for ( int i = 1; i < 2; ++i ) {
	char fname[1024];
	sprintf(fname, "Objects/Models/Models/Plants_and_Trees/M_TREE%d.3DS", i );
	Object * obj = ObjectLoader::LoadObject(fname, "");
	if ( obj ) {
	  BoundingBox b;
	  trees.push_back(obj);
	  Position p;
	  p.translate( Vector(0,0,0), true );
	  p.scale( MAX_TREE_HEIGHT, Position::SCALE_MAX );
	  p.transform(obj);
	  
	  obj->getBounds(b);
	  tboxes.push_back(b);

	  obj->preprocess();
	}
      }
      loadedTrees = true;
    }
  }

  bool wantTree(const Point2D &loc, double density )
  {
    // Use perlin noise in future.
    return drand48() <= density;
  }

  void addTree(Group *group, const Point &loc)
  {
    assert(group);
    assert(loadedTrees);
    unsigned which = unsigned(drand48() * trees.size());
    if ( which == trees.size() )
      which--;
    Object * tree = trees[which];
    assert(tree);
    BoundingBox bbox = tboxes[which];
    Matrix4 translateMatrix;
    translateMatrix.identity();
    translateMatrix.setCol( Vector(loc), 3 );
    group->addObject( new Instance( translateMatrix, tree ) ) ;
  }
  
  void addTrees(Group *group, Mountains * mtns,
		const Point2D &start, const Point2D &end,
		double density )
  {

    // Ensure trees are ready to add.
    loadTrees();

    // Figure out how granular we need to look to add trees - they all
    // need at least N feet of space.
    Vector2D diff = (end - start) / TREE_SPACING;

    // For each square, see if we want to add a tree
    for ( double x = start.x(); x < end.x(); x += diff.x() ) {
      for ( double y = start.y(); y < end.y(); y += diff.y() ) {
	// No trees if the square is underwater.
	Point2D minS(x,y);
	Point2D location(minS + diff * drand48() );
	if ( mtns && mtns->valueAt(location.x(), location.y()) <= WATER_LEVEL )
	  //if ( underwater( mtns, minS, minS + diff ) )
	  continue;

	if ( wantTree( location, density ) )
	  addTree( group, Point(location, mtns ? mtns->valueAt(location.x(),location.y()) : 0 ));
      }
    }
  }

  struct BridgeItem {
    BridgeItem(float c, const Point &p, float w, int n, bool v ) : currentX(c), start(p), width(w), numLanes(n), vertical(v) {}
    float currentX;
    Point start;
    float width;
    int numLanes;
    bool vertical;
  };
  typedef std::list<BridgeItem> BridgeItemList;
  BridgeItemList BridgeItems;
  bool HasBridge( float currentX, bool v ) {
    for (BridgeItemList::iterator it = BridgeItems.begin();
	 it != BridgeItems.end();
	 it++) {
      if (ABSCLOSE( currentX, it->currentX, 1e-2) && it->vertical == v ) {
	return true;
      }
    }
    return false;
  }
  BridgeItem PopBridge( float currentX, bool v ) {
    for (BridgeItemList::iterator it = BridgeItems.begin();
	 it != BridgeItems.end();
	 it++) {
      if (ABSCLOSE( currentX, it->currentX, 1e-2) && it->vertical == v ) {
	BridgeItem b = *it;
	BridgeItems.erase(it);
	//std::cerr << "Finishing " << (v ? "vertical" : "horizontal") << " bridge from " << currentX << std::endl;
	return b;
      }
    }
    ERROR("No bridge started at " << currentX );
    assert(0);
  }
  
  void StartBridge( float currentX, const Point &p, float w, int n, bool v ) {
    //std::cerr << "Starting " << (v ? "vertical" : "horizontal") << " bridge at " << currentX << " Start = " << p << " Width = " << w << std::endl;
    BridgeItems.push_back( BridgeItem(currentX, p, w, n, v) );
  }
  bool creatingVBridge = false;
  Point bridgeVStart;
  int numBridgeVLanes;
  bool creatingHBridge = false;
  Point bridgeHStart;
  int numBridgeHLanes;

  double roadTime = 0.0;
  double bridgeTime = 0.0;
  double waterTime = 0.0;
  double buildingTime = 0.0;
  double treeTime = 0.0;
  void makeSquareNew(Group *group, float currentX, float currentY,
		     float blkSizeX, float blkSizeY, float laneSize,
		     int yStreetLanes, int xStreetLanes,
		     Mountains * mtns) {
    Timer timer;
    if (!cityMod)
      loadCityMod(group, mtns);
    
    double location = 0.0;
    if ( mtns ) {
      location = mtns->valueAt( currentX + blkSizeX / 2.0,
				currentY + blkSizeY / 2.0 );
    }

    // Add an intersection if not below water.
    Point2D imin( currentX + blkSizeX, currentY + blkSizeY );
    Point2D imax( currentX + blkSizeX + xStreetLanes * laneSize,
		  currentY + blkSizeY + yStreetLanes * laneSize );


    if ( !underwater( mtns, imin, imax ) ) {
      timer.restart();
      makeIntersectionNew( imin, imax );
      roadTime += timer.time();
    }
    else {
      timer.restart();
      setupWaterLine( group, mtns, imin, imax );
      waterTime += timer.time();
    }

    // Add horizontal lane.
    Point2D hmin(currentX + blkSizeX, currentY);
    Point2D hmax(currentX + blkSizeX + xStreetLanes * laneSize,currentY + blkSizeY);
      
    if ( underwater( mtns, hmin, hmax ) ) {
      if ( xStreetLanes > MIN_BRIDGE_LANES ) {

	timer.restart();
	if ( !HasBridge(currentX, false) ) {
	  StartBridge(currentX, Point(hmin, 25.0), hmax.x(), xStreetLanes, false );
	}
	bridgeTime += timer.time();
      }
      else{
	timer.restart();
	setupWaterLine( group, mtns, hmin, hmax );
	waterTime += timer.time();
      }
    }
    else {
      timer.restart();
      if ( HasBridge(currentX, false) ) {
	BridgeItem b = PopBridge(currentX, false);
	makeBridge(group,
		   b.start, Point(hmax, 25.0), b.numLanes,
		   false );
      }
      bridgeTime += timer.time();
      timer.restart();
      makeLaneNew( hmin, hmax, false );
      roadTime += timer.time();
    }

    // Add vertical lane
    Point2D vmin( currentX,currentY + blkSizeY );
    Point2D vmax( currentX + blkSizeX, currentY + blkSizeY + yStreetLanes * laneSize );

    if ( underwater( mtns, vmin, vmax ) ) {
      if ( yStreetLanes > MIN_BRIDGE_LANES ) {
	timer.restart();
	if ( !HasBridge(currentY, true) ) {
	  StartBridge(currentY, Point(vmin, 25.0), vmax.y(), yStreetLanes, true);
	}
	bridgeTime += timer.time();
      }
      else {
	timer.restart();
	setupWaterLine( group, mtns, vmin, vmax );
	waterTime += timer.time();
      }
    }
    else {
      timer.restart();
      if ( HasBridge(currentY, true) ) {
	BridgeItem b = PopBridge(currentY, true);
	makeBridge(group,
		   b.start, Point(vmax.x(), b.width, 25.0), b.numLanes,
		   true );
      }
      bridgeTime += timer.time();
      timer.restart();
      makeLaneNew( vmin, vmax, true );
      roadTime += timer.time();
    }

    // Check for buildings
    double newLoc = location;
    if ( drand48() < SQUARES_WITH_BUILDINGS &&
	 !underwater( mtns,
		      Point2D( currentX, currentY ),
		      Point2D( currentX + blkSizeX, currentY + blkSizeY ),
		       &newLoc ) ) {
      timer.restart();
      fillSquareWithBuildingsC( group,
				Point( currentX, currentY, newLoc ),
				Point( currentX + blkSizeX, currentY + blkSizeY, newLoc ) );
      buildingTime += timer.time();
    }
    else {
      timer.restart();
      addTrees(group, mtns,
	       Point2D( currentX, currentY ),
	       Point2D(currentX + blkSizeX, currentY + blkSizeY ),
	       TREE_DENSITY );
      treeTime += timer.time();
      timer.restart();
      setupWaterLine( group, mtns,
		      Point2D( currentX, currentY ),
		      Point2D(currentX + blkSizeX, currentY + blkSizeY ) );
      waterTime += timer.time();
    }

  }

  void printAccumTime() {
    std::cerr << "Road time: " << roadTime << std::endl;
    std::cerr << "Bridge time: " << bridgeTime << std::endl;
    std::cerr << "Water time: " << waterTime << std::endl;
    std::cerr << "Building time: " << buildingTime << std::endl;
    std::cerr << "Tree time: " << treeTime << std::endl;
    std::cerr << "Total: " << (roadTime + bridgeTime + waterTime + buildingTime + treeTime) << std::endl;
  }
  %}
void printAccumTime();
void fillSquareWithBuildingsC( Group *group,
			       Point min, Point max );


void makeSquareNew(Group *group,
		   float currentX, float currentY,
		   float blkSizeX, float blkSizeY,
		   float laneSize,
		   int yStreetLanes, int xStreetLanes,
		   Mountains *mtns);

void spreadWater( Group *group,
		  Water *water,
		  Point min, Point max,
		  int xRes, int yRes, double extraOffset );

void setCityParams( const Point2D& center,
		    const Point2D& size );
 
void incarnateBridges( Group * group );
