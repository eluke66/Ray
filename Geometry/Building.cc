
#include "Geometry/Building.h"
#include "Geometry/ScaledBox.h"
#include "Geometry/BoundedPlane.h"
#include "Core/BoundingBox.h"
#include "Core/HitRecord.h"
#include "Core/Math.h"
#include "Core/Point.h"
#include "Core/Ray.h"
#include "Core/Vector.h"
#include "Core/Matrix.h"
#include "Geometry/Group.h"
#include "Geometry/Triangle.h"
#include "Core/utils.h"
#include "Material/BasicShade.h"

// Building Factory //////////////////
#include "Core/Scene.h"
#include "Material/OrenNayarMaterial.h"
#include "Material/LambertianMaterial.h"
#include "Material/BrickColor.h"
#include "Material/PhongMaterial.h"
#include "Material/Modulators.h"
#include "Material/MetalMaterial.h"
#include "Material/MaterialModulator.h"
#include "Geometry/Sphere.h"
#include "Geometry/ToppedBox.h"
//////////////////////////////////////

#include <iostream>
using namespace std;

#include "Geometry/CSG.h"
class CutBuilding : public Building {
public:
  CutBuilding(Material* sideMatl,
	      Material* topMatl,
	      Material* roof,
	      const Point& p1, const Point& p2) :
    Building(sideMatl, topMatl, roof, p1, p2), _csg(NULL), _cuts(NULL) {
  ALLOC;DALLOC(Building);
  }

  void addCut( Primitive *cut );
  virtual void preprocess();
  virtual void intersect(HitRecord& hit,
			 const RenderContext& context,
			 const Ray& ray) const;
protected:
  CSG *_csg;
  CSG_Item *_cuts;
};

void
CutBuilding::addCut(Primitive *cut)
{
  CSG_Primitive *prim = new CSG_Primitive(cut);

  if ( ! _cuts ) 
    _cuts = prim;
  else
    // 2nd or more. Combine these in an and.
    _cuts = new CSG_Or(_cuts, prim);
}

void 
CutBuilding::preprocess()
{
  // Final structure is:
  // this AND NOT cuts

  CSG_Item *item = CSG_Item::MakeCSG(_parts);
  CSG_Item *top = NULL;
  if ( item && _cuts ) {
    top = new CSG_And( item, new CSG_Not( _cuts ) );
    // Use this to show cuts top = new CSG_Or( item, _cuts );
    
  }
  else if ( !item ) {
    top = new CSG_Not( _cuts );
  }
  else {
    top = item;
  }
  
  _csg = new CSG(top);
}

void 
CutBuilding::intersect(HitRecord& hit,
		       const RenderContext& context,
		       const Ray& ray) const
{
  // Intersect the csg.
  _csg->intersect(hit,context,ray);
}

/*
  Entry: Full size
         Calc total number of levels

  Call DrawLevels( fullSize, totalNumberofLevels )

  DrawLevels( size, levelsLeft ):
    - Get height for level
    - Get number of subblocks for level (1-4).

*/

#include <queue>
struct D2 {
  D2(const Point &min, const Point &max ) : pmin(min), pmax(max) {}
  Point pmin, pmax;
};

Building::Building(Material* sideMatl,
		   Material* topMatl,
		   Material* roof,
		   const Point& p1, const Point& p2)
  : Primitive(sideMatl), _sideMatl(sideMatl), _topMatl(topMatl), _roof(roof)
{
  ALLOC;
  _parts = new Group;

  // Calculate size of non-windowed top and roof doodads.
  double topSize = fabs(GaussianRandom( 0.075 ));
  if ( topSize > 0.1 )
    topSize = 0.1;

  double zFactor = p2.z() * topSize;
  
  double top = p2.z() - zFactor;

  
  // Main building section(s)
  Point p3( p2.x(), p2.y(), top );
  Box * sb = new ScaledBox( sideMatl, p1, p3 );
  _parts->addObject(sb);

  // Top of main building section
  top = p3.z() + p2.z() * topSize;
  Point p4( p3.x(), p3.y(), top );
#ifdef NO_TOPPED_BOX
  _parts->addObject(new Box( topMatl, Point(p1.x(), p1.y(), p3.z()), p4 ) );
#else

  _parts->addObject(new ToppedBox(topMatl,
				  roof,
				  Point(p1.x(), p1.y(), p3.z()), p4 ) );

  _parts->addObject(new ToppedBox(topMatl, roof,
				  Point(p1.x(), p1.y(), p4.z()),
				  Point(p1.x() + 1, p4.y(), p4.z()+2) ) );
  _parts->addObject(new ToppedBox(topMatl, roof,
				  Point(p4.x()-1, p1.y(), p4.z()),
				  Point(p4.x(), p4.y(), p4.z()+2) ) );
  _parts->addObject(new ToppedBox(topMatl, roof,
				  Point(p1.x(), p1.y(), p4.z()),
				  Point(p4.x(), p1.y()+1, p4.z()+2) ) );
  _parts->addObject(new ToppedBox(topMatl, roof,
				  Point(p1.x(), p4.y()-1, p4.z()),
				  Point(p4.x(), p4.y(), p4.z()+2) ) );
				  
#endif
  
}

Building::~Building()
{
}

void Building::transform(const Matrix4 &matrix) {
  assert(_parts);
  _parts->transform(matrix);
}

void Building::getBounds(BoundingBox& bbox) const
{
  _parts->getBounds(bbox);
}

void Building::intersect(HitRecord& hit, const RenderContext&c, const Ray& ray) const
{
  _parts->intersect(hit,c,ray);
}

void Building::computeUVW(Vector& uvw, const RenderContext& context,
                           const Ray& ray, const HitRecord& hit) const
{
  const Primitive * p = hit.getPrimitive();
  assert(p);
  p->computeUVW( uvw, context, ray, hit );
}

void Building::getNormal(Vector& normal, RenderContext&c, const Point& hitpos,
                    const Ray& ray, const HitRecord& hit) const
{
  const Primitive * p = hit.getPrimitive();
  assert(p);
  p->normal(normal,c,hitpos,ray,hit);
}

void
Building::add( Object * object ) {
  _parts->addObject( object );
}

////////////////////////////////////////////////////////
float
BuildingFactory::_UnitsPerFoot = -1.0;
float
BuildingFactory::_MaxHeight = -1.0;

void
BuildingFactory::Initialize( float unitsPerFoot, float maxHeight ){
  _UnitsPerFoot = unitsPerFoot;
  _MaxHeight = maxHeight;
  assert( _UnitsPerFoot > 0.0 );
  assert( _MaxHeight > 0.0 );
  
}

double
BuildingFactory::GetBuildingHeight( BuildingType type,
				    const Point &min,
				    const Point &max )
{
  // Height of skyscraper as a function of the number of buildings at
  //  at least that height.
  //  h(n) = h0 - h1*log(n) with h0 = maxHeight and h1 = 150 ft
  // Min height is 20 ft.
  // At n = 1000, eqn should evaluate to 20
  // 20 = Max - h*3
  // h = (20-Max)/3
  // n ranges from 1 to 1000
  // For skyscrapers, n = 1 to 100
  // For midsize, n = 100 to 600
  // For brick, n = 600 to 1000

  double n = -1;
  switch ( type ) {
  case SKYSCRAPER:
    n = randRange( 1, 100 );
    break;
  case MIDSIZE:
    //return _MaxHeight * 0.5;
    n = randRange( 100, 600 );
    break;
  case BRICK_BUILDING:
    //return _MaxHeight * 0.2;
    n = randRange( 600, 1000 );
    break;

  default:
    assert(!"Bad building type!");
  }

  double h = (_MaxHeight - 20) / 3.0 ;
  double newH = _MaxHeight - h * log10(n);
  return newH;
}

/*
Concrete:
181,190,199
189,201,197
166,173,155
135,128,110
197,198,167
196,200,173
216,215,194
182,185,164
202,202,178
181,183,162

C/B:
236,240,213
164,145,102

Brick:
110,108,93
54,50,39
153,112,94
104,66,63
125,93,82
106,93,85
84,56,42

Steel:
33,57,61
35,29,31
145,143,148


====================================
Tallest are 50/50 concrete vs steel
Next are 50/35/15 brick/concrete/steel
Next are 80/15/5 brick/concrete/steel

*/
// Creates a Gaussian Distribution, but only with points inside 1
// standard deviation.
float GInStd( float mean, float stddev ) {
  
  float result = GaussianRandom( stddev, mean );
  float mms = mean-stddev;
  float mps = mean+stddev;
  
  while ( result <= mms || result >= mps )
    result = GaussianRandom( stddev, mean );

  return result;
}

//#define USE_OREN_NAYAR
Material * concrete(Color *c=NULL, double variance=1.0) {
  double red = 0.53 + variance*randRange( 0, 0.47 );
  // Green is +- 0.04 of red
  double green = red + variance*randRange( -0.04, 0.04 );
  double blue;
  // If it's more green than red, do extra blue.
  if ( green - red > 0.03 )
    blue = green + variance*randRange( 0, 0.03 );
  else
    blue = red * (0.83 + variance*randRange( 0, 0.07 ) );

  if (c) *c = Color(red,green,blue);
#ifdef USE_OREN_NAYAR
  return new OrenNayarMaterial( Color(red,green,blue),
				randRange( 0.05, 0.2 ) );
#else
  return new LambertianMaterial( Color( red, green, blue ),
				 randRange( 0.55, 0.65 ), 0.4 );
#endif
}

Material *
BuildingFactory::Concrete(double variance) {
  
  return concrete(NULL,variance);
}

typedef _Material<BrickColor, OrenNayarLighting> OrenNayarBrickMaterial;

Material * brick(double bwidth=1.0, double bheight=1.0,
		 double uRepeat=10.0, double vRepeat=100.0,
		 Color *lb=NULL, Color *db=NULL) {
  double red,green,blue;
  
  // Get red brick 2/3rds of the time.
  if ( drand48() < 0.666 ) {
    red = 0.41 + GInStd(0, 0.2);//randRange( 0, 0.4 );
    green = red - randRange( 0.1, (red*0.5) );
    blue = randRange( 0.7, 0.9 ) * green;
  }
  else {
    // Yellow brick - red and green are approximately equal.
    red = 0.6 + GInStd(0, 0.2);// + randRange( 0, 0.4 );
    green = red * randRange( 0.9, 1.1 );
    blue = green * randRange( 0.7, 0.85 );
  }
#ifdef NO_BRICK
  return new OrenNayarMaterial( Color(red,green,blue),
				randRange( 0.05, 0.2 ) );
#else
  Color light = Color( red,green,blue) * 1.5;
  Color dark = Color( red,green,blue) * 0.5;
  //std::cerr << "LIGHT: " << light << ", DARK: "<< dark << std::endl;
  BrickColor b( light, dark,
		drand48() < 0.1, // Only aligned ~ 10% of the time
		Point2D(bwidth, bheight),
		Color(1),
		uRepeat, vRepeat );
  if ( lb ) *lb=light;
  if ( db ) *db=dark;
  return new OrenNayarBrickMaterial( b, OrenNayarLighting(randRange( 0.05, 0.2 )) );
#endif
}

Material * steel(Color *c=NULL) {
  // Steel can be a shade of black or a shade of gray/silver, with a
  // tint of blue.
  double red, green, blue;
  double val = randRange( 0, 0.5 );
  red = val * randRange( 0.95, 1.05 );
  green = val * randRange( 0.95, 1.05 );
  blue = val * randRange( 1, 1.1 );

  if ( c ) *c = Color(red,green,blue);
  return new PhongMaterial( Color(red,green,blue),
			    0.6, 0.4,
			    Color(1,1,1),
			    40 + (int)randRange(0, 60) );
}



void
BuildingFactory::CreateMaterials( BuildingType type,
				  double hSize,
				  double vSize,
				  Material *&win,
				  Material *&nonWin,
				  Material *&roof )
{
  double which = drand48();
  // For now, all items get windows of the same type.
  Color winColor( drand48() );
  Material * winGlass = new MetalMaterial( winColor,
					   80 + int(20 * (drand48()-0.5)) );

  double hGap=0, vGap=0, hRepeat=0, vRepeat=0;
  Color c1, c2;
  
  switch ( type ) {
  case SKYSCRAPER:
    if ( which > 0.5 )
      nonWin = concrete(&c1);
    else
      nonWin = steel(&c1);

    hGap = GInStd( 0.3, 0.3 );
    vGap = GInStd( 0.3, 0.3 );

    // Average window size is 8 feet, plus an average of
    // 2-3 feet of concrete.
    hRepeat = (hSize / _UnitsPerFoot) / GaussianClamp( 20, 4, 11 );
    
    // Average story height is ~ 11-12 feet.
    vRepeat = (vSize / _UnitsPerFoot) / randRange( 9, 14 ); 
    break;
    
  case MIDSIZE:
    

    hGap = GInStd( 0.3, 0.3 );
    vGap = GInStd( 0.3, 0.3 );
    
    // Average window size is 8 feet, plus an average of
    // 2-3 feet of concrete.
    hRepeat = (hSize / _UnitsPerFoot) / GaussianClamp( 20, 4, 11 );
    
    // Average story height is ~ 11-12 feet.
    vRepeat = (vSize / _UnitsPerFoot) / randRange( 9, 14 ); 

    if ( which < 0.35 ) {
      nonWin = brick(1.0, 1.0,
		     hRepeat * 10, vRepeat * 10, &c1, &c2);
      c1 = (c1 + c2) * 0.5;
    }
    else if ( which < 0.85 )
      nonWin = concrete(&c1);
    else
      nonWin = steel(&c1);
    break;
  case BRICK_BUILDING:
    
    hGap = GInStd( 0.5, 0.2 );
    vGap = GInStd( 0.5, 0.2 );
    
    // Average window size is 4 feet, plus an average of
    // 6 feet of concrete.
    hRepeat = (hSize / _UnitsPerFoot) / GaussianClamp( 15, 4, 9 );

    // Average story height is ~ 11-12 feet.
    vRepeat = (vSize / _UnitsPerFoot) / randRange( 9, 15 );

    if ( which < 0.8 ) {
      nonWin = brick(1.0, 1.0,
		     hRepeat * 10, vRepeat * 10,
		     &c1, &c2);
      c1 = (c1 + c2) * 0.5;
    }
    else if ( which < 0.95 )
      nonWin = concrete(&c1);
    else
      nonWin = steel(&c1);

    break;
  default:
    assert(!"Bad building type!");
  }

  
  // Ensure that the repeats are integers, so we don't get any
  // half-windows sitting around.
  int h = (int)hRepeat;
  if ( h == 0 )
    h = 1;
  hRepeat = h;

  int v = (int)vRepeat;
  if ( v == 0 )
    v = 1;
  vRepeat = v;


  
  // Create a building modulator
  Modulator *modulator =
    new SimpleBuildingModulator( hGap,
				 vGap,
				 hRepeat,
				 vRepeat );
  MaterialList m;
  m.push_back( nonWin );
  m.push_back( winGlass );
  win = new MaterialModulator( modulator, m );

  // Create the roof material - concrete
  //roof = concrete();

  double red = 0.13 + randRange( 0, 0.25 );
  // Green is +- 0.04 of red
  double green = red + randRange( -0.04, 0.04 );
  double blue;
  // If it's more green than red, do extra blue.
  if ( green - red > 0.03 )
    blue = green + randRange( 0, 0.03 );
  else
    blue = red * (0.83 + randRange( 0, 0.07 ) );

  Color croof(red,green,blue);
  // Roof is average color of non-window material and the colors
  // created above.
  roof = new OrenNayarMaterial( (c1+croof) * 0.5,
				randRange( 0.05, 0.2 ) );
}

void
addBox1( const Point &p1, const Point &p2,
	 Building *b, Material *win, Material *top, Material *roof ) {
  
  const double EPS = 1e-2;

  // FIXME XXX
  // Not sure if this calculation is correct.
  BoundingBox bb;
  b->getBounds(bb);
  double scale = (p2-p1).length()/ bb.getSize();
  b->add( new ScaledBox( win, p1, Point(p2.x(), p2.y(), p2.z()-EPS), scale ) );
#ifdef NO_TOPPED_BOX
  b->add( new Box( top,
		   Point(p1.x(),p1.y(), p2.z()-EPS),
		   Point(p2.x(), p2.y(), p2.z()+EPS) ) );
#else

  b->add( new ToppedBox( top,
			 roof,
			 Point(p1.x(),p1.y(), p2.z()-EPS),
			 Point(p2.x(), p2.y(), p2.z()+EPS) ) );
  
  b->add(new ToppedBox(top, roof,
		       Point(p1.x(), p1.y(), p2.z()),
		       Point(p1.x() + 1, p2.y(), p2.z()+2) ) );
  b->add(new ToppedBox(top, roof,
		       Point(p2.x()-1, p1.y(), p2.z()),
		       Point(p2.x(), p2.y(), p2.z()+2) ) );
  b->add(new ToppedBox(top, roof,
		       Point(p1.x(), p1.y(), p2.z()),
		       Point(p2.x(), p1.y()+1, p2.z()+2) ) );
  b->add(new ToppedBox(top, roof,
		       Point(p1.x(), p2.y()-1, p2.z()),
		       Point(p2.x(), p2.y(), p2.z()+2) ) );
#endif
}

void
BuildingFactory::AddBuildingDetail( BuildingType type,
				    float distanceFactor,
				    const Point &min,
				    const Point &max,
				    Point &roofMin,
				    Point &roofMax,
				    Building *building,
				    Material *windows,
				    Material *nonWin,
				    Material *roof)
{
  Point p1(min);
  Point p2(max);
  
  // We grow the building by creating 1..N subsegments
  int numLevels = (int)GaussianClamp( 5, 1, 3 );
  
  // Adjust by distance factor
  // Adjust by type
  
  PRINT_TIMES("We have " << numLevels << " levels in this building!", 10);

  double currentLocation = p1.z();

  double avgZInc = (p2.z() - p1.z()) / numLevels;
  
  double zIncrement;
  std::queue<D2> blocks;
  blocks.push( D2( p1, p2 ) );
  
  for ( int whichLevel = 0; whichLevel != numLevels; ++whichLevel ) {

    // Get the height for this level.
    avgZInc = (p2.z() - currentLocation) / (numLevels - whichLevel);
    zIncrement = GaussianRandom( avgZInc, avgZInc / 5.0 );
    if ( zIncrement < avgZInc - avgZInc / 2.5 )
      zIncrement =  avgZInc - avgZInc / 2.5;
    if ( zIncrement > (p2.z() - currentLocation) )
      zIncrement = (p2.z() - currentLocation) * 0.5;

    // Subblocks for each level must be symmetrical. Alternatively, we
    // could have a small chance that they would be symmetrical only on
    // the X or Y axis.

    // TEMP int numBlocks = (int)(drand48() * 3 ) + 1;
    int numBlocks = (int)(drand48() * 1 ) + 1;
    int blocksAtThisLevel = blocks.size();
    D2 block = blocks.front();
    Vector psize = block.pmax - block.pmin;
    
    switch (numBlocks) {
      // 1 Block  - inset on x and/or y axis.
    case 1:
      {
	double xInset = psize.x() * ( 0.2 - 0.1 * drand48() );
	double yInset = psize.y() * ( 0.2 - 0.1 * drand48() );
	for ( int b = 0; b < blocksAtThisLevel; ++b ) {

	  Point pa( block.pmin.x(),
		    block.pmin.y(),
		    currentLocation );
	  Point pb( block.pmax.x(),
		    block.pmax.y(),
		    currentLocation + zIncrement );
	  roofMin = pa;
	  roofMax = pb;
	  addBox1( pa, pb, building, windows, nonWin, roof );
	  pa = Point( block.pmin.x() + xInset,
		      block.pmin.y() + yInset,
		      currentLocation );
	  pb = Point( block.pmax.x() - xInset,
		      block.pmax.y() - yInset,
		      currentLocation + zIncrement );
	  
	  blocks.pop();
	  blocks.push( D2( pa, pb ) );
	  block = blocks.front();
	}
      }
      break;
      
      // 2 Blocks - inset on axis/axes, on x or y axis.
    case 2:
      {
	double xInset = psize.x() * ( 0.2 - 0.1 * drand48() );
	double yInset = psize.y() * ( 0.2 - 0.1 * drand48() );
	
	for ( int b = 0; b < blocksAtThisLevel; ++b ) {

	  // Divide block into three parts
	  double xExtent = psize.x() / 3.0;
	  double yExtent = psize.y() / 3.0;

	  Point pleft, pright;
	  
	  // X biased
	  if ( drand48() > 0.5 ) {
	    pleft = Point( block.pmin.x() + xExtent,
			   block.pmax.y(),
			   currentLocation + zIncrement );
	    pright = Point( block.pmax.x() - xExtent,
			    block.pmin.y(),
			    currentLocation );
	  }
	  else {
	    // Y Biased
	    pleft = Point( block.pmax.x(),
			   block.pmin.y() + yExtent,
			   currentLocation + zIncrement );
	    pright = Point( block.pmin.x(),
			    block.pmax.y() - yExtent,
			    currentLocation );
	  }

	  Point pa, pb;

	  // Block #1 /////////////////////////
	  pa = Point( block.pmin.x(),
		      block.pmin.y(),
		      currentLocation );
	  pb = pleft;
	  addBox1( pa, pb,  building, windows, nonWin, roof );
	  pa = Point( pa.x() + xInset,
		      pa.y() + yInset,
		      pa.z() );
	  pb = Point( pb.x() - xInset,
		      pb.y() - yInset,
		      pb.z() );
	  blocks.push( D2( pa, pb ) );

	  // Block #2 /////////////////////////
	  pa = pright;
	  pb = Point( block.pmax.x(),
		      block.pmax.y(),
		      currentLocation + zIncrement);
	  
	  addBox1( pa, pb, building, windows, nonWin, roof );
	  pa = Point( pa.x() + xInset,
		      pa.y() + yInset,
		      pa.z() );
	  pb = Point( pb.x() - xInset,
		      pb.y() - yInset,
		      pb.z() );
	  blocks.push( D2( pa, pb ) );
	  ///////////////////////////////////////
	    
	  blocks.pop();
	  block = blocks.front();
	}
      }
      break;
      
      // 3 Blocks - inset on axes, blocks at ends and in middle
    case 3:
      break;
      
      // 4 Blocks - No inset, each at a corner
    case 4:
      break;

    default:
      std::cerr << "GRUUUUMP\n\n\n\n\n\n\n\n\n\n\n";
    }
    
    currentLocation += zIncrement;
  } // For level
  
}

typedef enum { NONE, EAVES, PYRAMID, BLOCK, DOME } RoofType;
static float SkyscraperRoofs[] = { 0.3, 0.1, 0.1, 0.45, 0.05 };
static float MidsizeRoofs[]    = { 0.3, 0.25, 0.15, 0.2, 0.1 };
static float BrickRoofs[]      = { 0.4, 0.4, 0.05, 0.1, 0.05 };

RoofType getRoofType( float roofs[] ) {
  int nRoofs = sizeof(SkyscraperRoofs) / sizeof(float);
  double which = drand48();
  double current = 0;
  for ( int i = 0; i < nRoofs; ++i ) {
    current += roofs[i];
    if ( which <= current )
      return (RoofType)i;
  }

  assert( !"No roof type!" );
  return NONE;
}

void
BuildingFactory::AddRooftop( BuildingType type,
			     float distanceFactor,
			     double height,
			     const Point &roofMin,
			     const Point &roofMax,
			     Building *building,
			     Material *windows,
			     Material *topMatl )
{

  // Figure out what type of roof ornamentation to create.
  // Types are: None, eaves, pyramid, block, dome
  RoofType roof;
  BoundingBox bounds;
  building->getBounds(bounds);
  Point p1( roofMin.x(), roofMin.y(), bounds.getMin().z() );
  Point p2( roofMax.x(), roofMax.y(), bounds.getMax().z() );
  double top = p2.z();
  
  // Not yet needed...double roofSize = height - (p2.z() - p1.z());
  
  switch ( type ) {
  case SKYSCRAPER:
    roof = getRoofType( SkyscraperRoofs );
    break;
  case MIDSIZE:
    roof = getRoofType( MidsizeRoofs );
    break;
  case BRICK_BUILDING:
    roof = getRoofType( BrickRoofs );
    break;
  default:
    roof = NONE;
  }

  // Now, based on the type of roof, add the decoration to the
  // building.
  switch ( roof ) {
  case EAVES:
    {
      double h1 = (p2.z() - p1.z()) / 80.0;
      double h2 = h1;
      // Overhangs range from 1 to 6 feet.
      double oh = randRange( 1, 6.0 ) * _UnitsPerFoot;
      double topP = p2.z() + h1 + h2;

      // roof eave top
      Point pmin( p1.x()-oh, p1.y()-oh, top+h1 );
      Point pmax( p2.x()+oh, p2.y()+oh, topP );
      building->add( new Box( topMatl,
			      pmin,
			      pmax ) );


      // 0-3 are on top of building
      // 4-7 are on base of roof
      Point pts[8];
      
      
      pts[0] = Point( p1.x(), p1.y(), top );
      pts[1] = Point( p2.x(), p1.y(), top );
      pts[2] = Point( p2.x(), p2.y(), top );
      pts[3] = Point( p1.x(), p2.y(), top );
      
      pts[4] = Point( p1.x()-oh, p1.y()-oh, p2.z()+h2 );
      pts[5] = Point( p2.x()+oh, p1.y()-oh, p2.z()+h2 );
      pts[6] = Point( p2.x()+oh, p2.y()+oh, p2.z()+h2 );
      pts[7] = Point( p1.x()-oh, p2.y()+oh, p2.z()+h2 );
      
      building->add( new Triangle( topMatl, pts[0], pts[4], pts[5] ) );
      building->add( new Triangle( topMatl, pts[0], pts[5], pts[1] ) );
      
      building->add( new Triangle( topMatl, pts[1], pts[5], pts[2] ) );
      building->add( new Triangle( topMatl, pts[2], pts[5], pts[6] ) );
      
      building->add( new Triangle( topMatl, pts[2], pts[6], pts[7] ) );
      building->add( new Triangle( topMatl, pts[2], pts[7], pts[3] ) );
      
      building->add( new Triangle( topMatl, pts[3], pts[7], pts[0] ) );
      building->add( new Triangle( topMatl, pts[0], pts[7], pts[4] ) );
    }
    break;
  case PYRAMID:
    {
      Point pts[4];
      double pHeight = (p2.z() - p1.z()) * 0.10;
      
      Point pTop( (p2.x()+p1.x()) / 2.0,
		  (p2.y()+p1.y()) / 2.0,
		  p2.z() + pHeight);

      // The inset is either 10% of the roof size, or enough to make
      // the base at most 200% of the pyramid height.
      double minDim = std::min(p2.x() - p1.x(), p2.y() - p1.y() );
      if ( drand48() > 10.5 ) {
	double inset = minDim / 10.0;
	
	pts[0] = Point( p1.x()+inset, p1.y()+inset, top );
	pts[1] = Point( p2.x()-inset, p1.y()+inset, top );
	pts[2] = Point( p2.x()-inset, p2.y()-inset, top );
	pts[3] = Point( p1.x()+inset, p2.y()-inset, top );

	building->add( new Triangle( topMatl, pts[0], pts[1], pTop ) );
	building->add( new Triangle( topMatl, pts[1], pts[2], pTop ) );
	building->add( new Triangle( topMatl, pts[2], pts[3], pTop ) );
	building->add( new Triangle( topMatl, pts[3], pts[0], pTop ) );
      }
      else {
	// Here, we're making a little hat. For every N that one
	// dimension is larger than another, create another little
	// hat.
	double dx = p2.x() - p1.x();
	double dy = p2.y() - p1.y();
	double N = 0.5;
	int nPyX = 1;
	int nPyY = 1;
	if ( dx > dy )
	  nPyX += int((dx / dy) / N);
	else
	  nPyY += int((dy / dx) / N);
	
	double baseSize = 5.0 * pHeight;
	baseSize = std::min( std::min( baseSize, dx ), dy );
	
	double xInset = (dx - nPyX * baseSize) / (nPyX + 1);
	while ( xInset < 0 ) {
	  nPyX--;
	  xInset = (dx - nPyX * baseSize) / (nPyX + 1);
	}
	  
	double yInset = (dy - nPyY * baseSize) / (nPyY + 1);
	while ( yInset < 0 ) {
	  nPyY--;
	  yInset = (dy - nPyY * baseSize) / (nPyY + 1);
	}
	assert( nPyY > 0 );
	assert( nPyX > 0 );

	for ( int x = 0; x < nPyX; ++x )	  
	  for ( int y = 0; y < nPyY; ++y ) {
	    double xStart = p1.x() + (x+1) * xInset + x*baseSize;
	    double xEnd = xStart + baseSize;
	    double yStart = p1.y() + (y+1) * yInset + y*baseSize;
	    double yEnd = yStart + baseSize;

	    pts[0] = Point( xStart,
			    yStart,
			    top );
	    
	    pts[1] = Point( xEnd,
			    yStart,
			    top );
	    
	    pts[2] = Point( xEnd,
			    yEnd,
			    top );
	    
	    pts[3] = Point( xStart,
			    yEnd,
			    top );

	    
	    pTop = Point( (xEnd+xStart) / 2.0,
			  (yEnd+yStart) / 2.0,
			  p2.z() + pHeight);
	    building->add( new Triangle( topMatl, pts[0], pts[1], pTop ) );
	    building->add( new Triangle( topMatl, pts[1], pts[2], pTop ) );
	    building->add( new Triangle( topMatl, pts[2], pts[3], pTop ) );
	    building->add( new Triangle( topMatl, pts[3], pts[0], pTop ) );
	  }
      }
    }
    break;
  case BLOCK:
    {
      double xSize = (p2.x() - p1.x()) * (drand48()*0.5+0.25);
      double xInset = ( (p2.x() - p1.x()) - xSize ) / 2.0;
      double ySize = (p2.y() - p1.y()) * (drand48()*0.5+0.25);
      double yInset = ( (p2.y() - p1.y()) - ySize ) / 2.0;
      
      Point p5( p1.x() + xInset, p1.y() + yInset, top );

      // Boxes range from 8 to 20 feet high, Gaussianly.
      double units = GaussianClamp(20, 4, 8) * _UnitsPerFoot;
      Point p6( p1.x() + xInset + xSize,
		p1.y() + yInset + ySize,
		p2.z() + units);
      building->add( new Box( topMatl, p5, p6 ) );
    }
    break;
  case DOME:
    {
      // Find the minimum dimension
      double minDim = std::min(p2.x() - p1.x(), p2.y() - p1.y() );
      
      // Create a little inset
      double inset = minDim * randRange( 0.05, 0.1 );
      
      // Create a sphere of that radius
      building->add( new Sphere( topMatl,
				 Point( ( p2.x() + p1.x() ) / 2.0,
					( p2.y() + p1.y() ) / 2.0,
					p2.z() ),
				 (minDim - 2*inset) / 2.0 ) );
    }
    break;

    // Both of these are no-ops
  case NONE:
  default:
    break;
  }
  

}

void
BuildingFactory::GetNonDecoratedSize( Point &ndMin,
				      Point &ndMax,
				      BuildingType type,
				      const Point &min,
				      const Point &max )
{
  // Temporary for now:
  Vector v = (max-min);
  Vector i( v.x() * randRange( 0, 0.5 ),
	    v.y() * randRange( 0, 0.5 ),
	    0 );
  ndMin = min + i;
  ndMax = max - i;
}

BuildingFactory::BuildingType
BuildingFactory::RandomBuilding() {
  return (BuildingType)randRange(0,NUM_BUILDING_TYPES);
}


Building *
BuildingFactory::CreateBuilding( BuildingType type,
				 const Point &min,
				 const Point &max,
				 float distanceFactor )
{
  // Ensure we're initialized.
  if ( _MaxHeight < 0.0 ) {
    assert( !"Cannot create building - factory is uninitialized!" );
  }
  
  // Figure out height of building based on type, min, and max.
  double height = GetBuildingHeight( type, min, max );

  // Get size of non-decorated building.
  Point ndMin, ndMax;
  GetNonDecoratedSize( ndMin, ndMax, type, min, max );
  
  // Based on building type, create non-window material and window
  // material. 
  Material *windows, *nonWin, *roof;
  double hSize = max.x() - min.x();
  CreateMaterials( type, hSize, height, windows, nonWin, roof );
  
  // Create initial building
  double averageHeight = (max.z() + min.z()) / 2.0;
  height += averageHeight;

  // This isn't turned on right now until we can find out why cutouts
  // leave big black boxes in the scene. It doesn't happen in the test
  // scene...maybe just shadows?
#define NEW_CUTAWAY
#ifdef NEW_CUTAWAY
  double which = drand48();
  Building *building = NULL;
  if ( which > 0.7 ) {
    CutBuilding *build = new CutBuilding(windows, nonWin, roof,
					 Point( ndMin.x(), ndMin.y(), averageHeight ),
					 Point( ndMax.x(), ndMax.y(), height ) );
    
    // Box cut
    int numCuts = (int)(3*drand48());
    for ( int i = 0 ; i < numCuts; i++ ) {
      double bwidth = randRange( 0.25, 0.33 );
      double boffset = randRange( 0.0, 1.0 - bwidth );
      double width = max.x() - min.x();//ndMax.x() - ndMin.x();
      double widthStart = ndMin.x() + boffset*width;
      double bdepth = randRange( 0.15, 0.5 );
      double depth = ndMax.y() - ndMin.y();
      Point p1(//ndMin.x() + width * bwidth/2.0,
	       widthStart,
	       ndMin.y()-4,
	       averageHeight );
      Point p2(//ndMax.x() - width * bwidth/2.0,
	       widthStart + width*bwidth,
	       ndMax.y() - bdepth*depth,
	       max.z()+height+100 );
#ifdef VERBOSE_CRAP
      std::cerr << "Building is " << Point( ndMin.x(), ndMin.y(), averageHeight ) << ", "
		<< Point( ndMax.x(), ndMax.y(), height ) << std::endl;
      std::cerr << "inset is " << p1 << " " << p2 << std::endl;
#endif
      Box *box = new Box( NULL,
			  p1,
			  p2 );
      build->addCut(box);
    }
    building = build;
    
  }
  else {
    building = new Building( windows, nonWin, roof,
			     Point( ndMin.x(), ndMin.y(), averageHeight ),
			     Point( ndMax.x(), ndMax.y(), height ) );
  }
#else
  Building * building = new Building( windows, nonWin, roof,
				      Point( ndMin.x(), ndMin.y(), averageHeight ),
				      Point( ndMax.x(), ndMax.y(), height ) );
#endif
  // Based on building type and distance factor, augment building with
  // extra detail.
  Point roofMin, roofMax;

  AddBuildingDetail( type, distanceFactor,
		     min, Point(max.x(), max.y(), height ),
		     roofMin, roofMax,
		     building, windows, nonWin, roof );

  // Based on building type and distance factor, augment building with
  // rooftop additions.
  AddRooftop( type, distanceFactor, height,
	      //roofMin, roofMax,
	      ndMin, ndMax,
	      building, windows, nonWin );

  // Return the building.
  return building;

}
