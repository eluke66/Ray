
#include "Geometry/Bridge.h"
#include "Geometry/Box.h"
#include "Geometry/Sphere.h"
#include "Geometry/Cylinder.h"
#include "Geometry/Instance.h"
#include "Geometry/CSG.h"
#include "Core/Position.h"
#include "Core/BoundingBox.h"
#include "Core/HitRecord.h"
#include "Core/Math.h"
#include "Core/Point.h"
#include "Core/Ray.h"
#include "Core/Vector.h"
#include "Core/Matrix.h"
#include "Geometry/GriddedGroup.h"
#include "Geometry/Sphere.h"
#include "Core/utils.h"
#include "Material/BasicShade.h"
#include "Material/LambertianMaterial.h"
#include "Material/PhongMaterial.h"

#include <iostream>
using namespace std;


Bridge::Bridge(Material * matl, const Point& minPoint, const Point& maxPoint, double height,
	       double landOrWater )
  : Primitive(matl), _parts(new Group()), _minPoint(minPoint), _maxPoint(maxPoint),
    _height(height), _landOrWater(landOrWater), 
    _supportMatl(NULL), _towerMatl(NULL),
    idx(0), oidx(1)
{
  ALLOC;
  if ( maxPoint.x()-minPoint.x() < maxPoint.y()-minPoint.y() ) {
    idx = 1; // Vertical
    oidx = 0;
  }

}

Bridge::~Bridge()
{
  delete _parts;
}

void Bridge::transform(const Matrix4 &matrix) {
  assert(_parts);
  _parts->transform(matrix);
}

void Bridge::getBounds(BoundingBox& bbox) const
{
  _parts->getBounds(bbox);
}

void Bridge::intersect(HitRecord& hit, const RenderContext&c, const Ray& ray) const
{
  _parts->intersect(hit,c,ray);
}

void Bridge::computeUVW(Vector& uvw, const RenderContext& context,
                           const Ray& ray, const HitRecord& hit) const
{
  const Primitive * p = hit.getPrimitive();
  assert(p);
  p->computeUVW( uvw, context, ray, hit );
}

void Bridge::getNormal(Vector& normal, RenderContext&c, const Point& hitpos,
                    const Ray& ray, const HitRecord& hit) const
{
  const Primitive * p = hit.getPrimitive();
  assert(p);
  p->normal(normal,c,hitpos,ray,hit);
}


void
Bridge::preprocess() {

  if ( _supportMatl == NULL ) {
    _supportMatl = new LambertianMaterial( Color( .53, 0.57, .48 ),
					   0.52, 0.4 );
  }
  if ( _towerMatl == NULL ) {
    _towerMatl = new PhongMaterial( Color( 0.77, 0.74, 0.71 ),
				    0.6, 0.4, Color(1,1,1), 50 );
  }
}

Group * 
Bridge::createRoadbed(double railThickness,
		      const Point &minPoint,
		      const Point &maxPoint) {

  Group * parts = new Group();

  //////////////////////////////////////////////////////////////
  //
  // Create the roadbed
  //
  ///////////////////////////////////////////////////////////////
  double roadThickness = (maxPoint.idx(oidx) - minPoint.idx(oidx)) / 50.0;
  Point roadBedMin( minPoint.x(), minPoint.y(), minPoint.z() - roadThickness );
  Point roadBedMax( maxPoint.x(), maxPoint.y(), maxPoint.z() + roadThickness );

  parts->addObject(new Box(matl, roadBedMin, roadBedMax));

  // Side Rails
  Point sideMin(0,0,minPoint.z() - railThickness*0.2);
  Point sideMax(0,0,maxPoint.z() + railThickness*0.1);
  sideMin.setIdx(idx, minPoint.idx(idx));
  sideMin.setIdx(oidx, minPoint.idx(oidx) - railThickness);
  sideMax.setIdx(idx, maxPoint.idx(idx));
  sideMax.setIdx(oidx, minPoint.idx(oidx));
  parts->addObject(new Box(_towerMatl, sideMin, sideMax ));

  sideMin.setIdx(oidx, maxPoint.idx(oidx));
  sideMax.setIdx(oidx, maxPoint.idx(oidx) + railThickness);
  parts->addObject(new Box(_towerMatl, sideMin, sideMax ));
  
  ////////////// Scaffolding //////////////////////
  double angle = 60;
  double angleRad = DEGTORAD(angle);
  double ninetyMinus = DEGTORAD(90) - angleRad;
  double top = sideMin.z();
  double bottom = sideMin.z()-railThickness*2.0;
  double thick = railThickness*0.3;//railThickness;
  double scaffHeight = (top-bottom)/cos(ninetyMinus) + 2.0*thick / tan(angleRad);
  double dx = 0.5*scaffHeight*cos(angleRad);
  double stride = thick+2.0*dx;
  double sThick = thick / 3.0;
  Material * scafMat = _towerMatl;
  Box * box = new Box(scafMat,
		      Point(-sThick*0.5,
			    -sThick*0.5,
			    0),
		      Point(sThick*0.5,
			    sThick*0.5,
			    scaffHeight));

  double vOffset = thick * sin(ninetyMinus);
  double zTranslate = bottom - vOffset + (top-bottom+thick) * 0.5;

  double xMin = minPoint.idx(idx);
  double xMax = maxPoint.idx(idx);
  double yMin = minPoint.idx(oidx) - railThickness * 0.5;
  double yMax = minPoint.idx(oidx);

  GriddedGroup * scaffold = new GriddedGroup(10);
  int numScaffolds = 1 + int((xMax-xMin-0.5*thick-dx) / stride);
  for ( int i = 0; i < numScaffolds; i++ ) {
    Position p;
    Vector vr(0.0);
    vr.setIdx(oidx,90+angle);
    p.rotate(vr);
    Vector vt( zTranslate );
    vt.setIdx( idx,  xMin+thick*0.5+dx+i*stride );
    vt.setIdx( oidx, yMin+(yMax-yMin) / 2.0 );
    p.translate( vt, true );
    scaffold->addObject(new Instance( p.getMatrix(box),
				    box,
				    scafMat ));
    p.reset();
    vr.setIdx(oidx,90-angle);
    p.rotate( vr );
    p.translate( vt, true );
    scaffold->addObject(new Instance( p.getMatrix(box),
				    box,
				    scafMat ));
  }

  Point sp1(bottom-2.0*sThick);
  sp1.setIdx(idx, xMin);
  sp1.setIdx(oidx, yMin);

  Point sp2(bottom-sThick);
  sp2.setIdx(idx, xMax);
  sp2.setIdx(oidx, yMax);
  scaffold->addObject( new Box( scafMat,
				sp1,
				sp2 ) );
  parts->addObject(scaffold);
  
  scaffold = new GriddedGroup(10);

  yMin = maxPoint.idx(oidx) + railThickness * 0.5;
  yMax = maxPoint.idx(oidx);
  
  for ( int i = 0; i < numScaffolds; i++ ) {
    Position p;

    Vector vr(0.0);
    vr.setIdx(oidx,90+angle);
    p.rotate(vr );
    Vector vt( zTranslate );
    vt.setIdx( idx,  xMin+thick*0.5+dx+i*stride );
    vt.setIdx( oidx, yMin+(yMax-yMin) / 2.0 );
    p.translate( Vector( vt ), true );
    scaffold->addObject(new Instance( p.getMatrix(box),
				      box,
				      scafMat ));
    p.reset();
    vr.setIdx(oidx, 90-angle);
    p.rotate(vr );
    p.translate( vt, true );
    scaffold->addObject(new Instance( p.getMatrix(box),
				      box,
				      scafMat ));
  }
  sp1.setIdx(idx, xMin);
  sp1.setIdx(oidx, yMin);
  
  sp2.setIdx(idx, xMax);
  sp2.setIdx(oidx, yMax);
  scaffold->addObject( new Box( scafMat,
				sp1,
				sp2 ) );
  parts->addObject(scaffold);

  ////////////////////////////////////////////////

  ////////// Under-road structure.
  Point urMin;
  Point urMax( sideMax );
  urMin.setIdx(idx, minPoint.idx(idx));
  urMin.setIdx(oidx, minPoint.idx(oidx) - railThickness*0.25 );
  urMin.setIdx(2, bottom+sThick);
  urMax.setIdx(oidx, maxPoint.idx(oidx) + railThickness*0.25 );
  parts->addObject( new Box(_towerMatl, urMin, urMax ) );
  
  
  return parts;
}

void
Bridge::makeRoadbed(double railWidth) {

  _parts->addObject( createRoadbed( railWidth, _minPoint, _maxPoint ) );

  //Material *tmp = _towerMatl;
  //_towerMatl = new BasicShade(Color(1,0,0));
  
  Point minPt(_minPoint);
  Point maxPt(_maxPoint);

  
  double angle = 10;
  if ( idx == 1 )
    angle = -angle;
  double verticalLen = _minPoint.z() - _landOrWater;
  // sin(a) = o/h h = o/sina
  double sideLen = verticalLen / sin(DEGTORAD(angle));
  
  minPt.setIdx( idx, _minPoint.idx(idx) - sideLen );
  maxPt.setIdx( idx, _minPoint.idx(idx) );
  Group * left = createRoadbed( railWidth, minPt, maxPt );
  left->preprocess();
  Position p;
  Vector vr(0.0);
  vr.setIdx(oidx,angle);
  p.rotate(vr);

  BoundingBox bbox;
  left->getBounds(bbox);
  Point transformedMin = p.getMatrix(left) * bbox.getMin();
  Point transformedMax = p.getMatrix(left) * bbox.getMax();
  
  Vector vvt(0,0,-(transformedMax.z()-_minPoint.z()));
  vvt.setIdx(idx,-transformedMax.idx(idx) + _minPoint.idx(idx));
  p.translate(vvt,false);
  _parts->addObject( new Instance(p.getMatrix(left),
				  left,
				  NULL ) );
  //p.transform(left);
  //_parts->addObject(left);

  
  minPt.setIdx( idx, _maxPoint.idx(idx) );
  maxPt.setIdx( idx, _maxPoint.idx(idx) + sideLen );
  
  left = createRoadbed( railWidth, minPt, maxPt );
  left->preprocess();
  p.reset();
  vr.setIdx(oidx,-angle);
  p.rotate(vr);
  vvt.setIdx(idx,transformedMax.idx(idx) - _minPoint.idx(idx));
  p.translate(vvt,false);

  _parts->addObject( new Instance(p.getMatrix(left),
				  left,
				  NULL ) );
  //p.transform(left);
  // _parts->addObject(left);
}

PlatformBridge::PlatformBridge(Material *matl, const Point& minPoint, const Point& maxPoint,
			       double height, double landOrWater) :
  Bridge(matl,minPoint,maxPoint,height,landOrWater)
{
  ALLOC;
  DALLOC(Bridge);
}

PlatformBridge::~PlatformBridge() {

}

void
PlatformBridge::preprocess() {
  Bridge::preprocess();

  makeRoadbed( 1.0 );

  double supportWidth = (_maxPoint.idx(oidx) - _minPoint.idx(oidx)) * 0.3;
  
  // Add supports
  int numSupports = 5;
  Point2D minPoint(_minPoint.x(), _minPoint.y());
  Point2D maxPoint(_maxPoint.x(), _maxPoint.y());
  maxPoint.setIdx(idx, _minPoint.idx(idx) + supportWidth);

  Point2D pointOffset( 0.0 );
  pointOffset.setIdx( idx,  (_maxPoint.idx(idx) - _minPoint.idx(idx) - supportWidth) / double(numSupports) );

  for ( int i = 0; i <= numSupports; i++ ) {
    Point p1(minPoint, _landOrWater );
    Point p2(maxPoint, _minPoint.z() );
    assert(_supportMatl);
    Box * support = new Box(_supportMatl,
			    p1,
			    p2 );
    minPoint += pointOffset;
    maxPoint += pointOffset;
    _parts->addObject(support);
  }
  
  _parts->preprocess();
}

SuspensionBridge::SuspensionBridge(Material * matl, const Point& minPoint, const Point& maxPoint, double height,
				   double landOrWater, double spacingBetweenCables, int numTowers )
  
  : Bridge(matl, minPoint, maxPoint, height, landOrWater),
    _cableSpacing(spacingBetweenCables), _mainCableThickness(0.1), _verticalCableThickness(0.05),
    _towerThickness(1.0),
    _numTowers(numTowers),
    _towerType(SQUARE_ONE)
{
  ALLOC;
  DALLOC(Bridge);
  assert(_numTowers >= 2);
}

SuspensionBridge::~SuspensionBridge() {
}

void
SuspensionBridge::preprocess() {

  Bridge::preprocess();
  
  // Default tower width
  double towerWidth = _towerThickness;
  double towerOffsetFromSides = (_maxPoint.idx(idx) - _minPoint.idx(idx)) * 0.15;
  
  //////////////////////////////////////////////////////////////
  //
  // Create the roadbed
  //
  ///////////////////////////////////////////////////////////////
    makeRoadbed( towerWidth );
  
  //////////////////////////////////////////////////////////////
  //
  // Create the towers
  //
  ///////////////////////////////////////////////////////////////
  double totalSpace = _maxPoint.idx(idx) - _minPoint.idx(idx) - 2.0*towerOffsetFromSides;
  double spaceBetweenOffsets = totalSpace / double(_numTowers - 1);

  Material * cableMatl = _towerMatl;
  
  for ( int i = 0; i < _numTowers; i++ ) {
    Point2D towerStart;
    Point2D towerEnd;

    towerStart.setIdx( idx, _minPoint.idx(idx) + i*spaceBetweenOffsets + towerOffsetFromSides );
    towerStart.setIdx( oidx, _minPoint.idx(oidx) - towerWidth );
    towerEnd.setIdx( idx, towerStart.idx(idx) + towerWidth );
    towerEnd.setIdx( oidx, _maxPoint.idx(oidx) + towerWidth );

    _parts->addObject( makeTower( towerStart,
				  towerEnd,
				  _minPoint.z(),
				  _height,
				  _landOrWater ) );

    Point2D nearCableStart;
    Point2D nearCableEnd;

    nearCableStart.setIdx( idx, towerEnd.idx(idx) );
    nearCableStart.setIdx( oidx, towerStart.idx(oidx) + towerWidth * 0.5 );
    nearCableEnd.setIdx( idx, nearCableStart.idx(idx) + spaceBetweenOffsets - towerWidth );
    nearCableEnd.setIdx( oidx, nearCableStart.idx(oidx) );

    Point start( nearCableStart, _height );
    Point end( nearCableEnd, _height );
    if ( i == 0 ) {
      _parts->addObject( makeInitialAnchorCables( start, end, _minPoint.z(), cableMatl,
						  Point2D(_minPoint.idx(idx), _minPoint.idx(idx)+towerOffsetFromSides ) ) );
    }
    if ( i == (_numTowers-1) ) {
      _parts->addObject( makeFinalAnchorCables( start, end, _minPoint.z(), cableMatl,
						Point2D(_maxPoint.idx(idx),
							towerEnd.idx(idx) ) ) );
							
    }
    else {
      _parts->addObject( makeIntertowerCables( start, end, _minPoint.z(), cableMatl ) );
    }

    start.setIdx( oidx, _maxPoint.idx(oidx) - towerWidth * 0.5 );
    end.setIdx( oidx, _maxPoint.idx(oidx) - towerWidth * 0.5 );
    if ( i == 0 ) {
      _parts->addObject( makeInitialAnchorCables( start, end, _minPoint.z(), cableMatl,
						  Point2D(_minPoint.idx(idx), _minPoint.idx(idx)+towerOffsetFromSides ) ) );
    }
    if ( i == (_numTowers-1) ) {
      _parts->addObject( makeFinalAnchorCables( start, end, _minPoint.z(), cableMatl,
						Point2D(_maxPoint.idx(idx), towerEnd.idx(idx)) ) );
    }
    else {
      _parts->addObject( makeIntertowerCables( start, end, _minPoint.z(), cableMatl ) );
    }

  }
  _parts->preprocess();
}


double
SuspensionBridge::chainHeight( double x ) {
  double zmk = x - _midX;
  return zmk*zmk / _a + _midZ;
}

void 
SuspensionBridge::addVerticalCables( double startX, double endX, double bottom, double Y, Group *group ) {

  int numCables = int((endX - startX) / _cableSpacing);
  double dx = (endX - startX) / numCables;
  
  for ( int i = 1; i < numCables; i++ ) {
    double x = startX + dx*i;
    Cylinder * cyl = new Cylinder( matl,
				   chainHeight( x ) - bottom,
				   _verticalCableThickness );
    Position p;
    if ( idx == 0 ) {
      p.translate( Vector(x,
			  Y,
			  bottom), false );
    }
    else {
      p.translate( Vector(Y,
			  x,
			  bottom), false );
    }
    p.transform(cyl);
    group->addObject( cyl );
  }
}

Group * 
SuspensionBridge::makeIntertowerCables( const Point &startPoint, const Point &endPoint, double bottom, Material *matl ) {

  Group * group = new GriddedGroup(10);

  _midX = (endPoint.idx(idx) - startPoint.idx(idx)) / 2.0 + startPoint.idx(idx);
  _midZ = bottom + endPoint.z() / 10.0;

  /*
    dx^2 = 4*a*dz
    
    a = dx^2 / 4*dz
    
   */
 
  double ymk = startPoint.idx(idx) - _midX;
  _a = (ymk*ymk) / (startPoint.z() - _midZ);

  createCable( startPoint, endPoint, matl, group );

  // Now create vertical cables
  addVerticalCables( startPoint.idx(idx), endPoint.idx(idx), bottom, startPoint.idx(oidx), group );
  return group;
}

Group * 
SuspensionBridge::makeInitialAnchorCables(  const Point &startPoint, const Point &endPoint, double bottom, Material *matl, const Point2D &initial ) {

  Group * group = new GriddedGroup(10);

  _midZ = bottom + endPoint.z() / 10.0;
  
  // Initial: x = far left
  //          y = joint on left side of left tower.
  _midX = initial.x();
  double ymk = initial.y() - _midX;
  _a = (ymk*ymk) / (startPoint.z() - _midZ);

  
  if ( idx == 0 ) {
    createCable( Point(initial.x(), startPoint.idx(oidx), _midZ),
		 Point(initial.y(), startPoint.idx(oidx), startPoint.z()), matl, group );
  }
  else {
    createCable( Point(startPoint.idx(oidx), initial.x(), _midZ),
		 Point(startPoint.idx(oidx), initial.y(), startPoint.z()), matl, group );
  }

  return group;
}

Group * 
SuspensionBridge::makeFinalAnchorCables(  const Point &startPoint, const Point &endPoint, double bottom, Material *matl, const Point2D &final ) {

  Group * group = new GriddedGroup(10);

  _midZ = bottom + endPoint.z() / 10.0;
  
  // Final: x = far right
  //          y = joint on right side of right tower.
  _midX = final.x();
  double ymk = final.y() - _midX;
  _a = (ymk*ymk) / (startPoint.z() - _midZ);

  if ( idx == 0 ) {
    createCable( Point(final.x(), endPoint.y(), _midZ),
		 Point(final.y(), endPoint.y(), endPoint.z()), matl, group );
  }
  else {
    createCable( Point(endPoint.idx(oidx), final.x(), _midZ),
		 Point(endPoint.idx(oidx), final.y(), endPoint.z()), matl, group );
  }
  
  addVerticalCables( final.y(), final.x(), bottom, endPoint.idx(oidx), group );
  return group;
}

Group *
SuspensionBridge::makeCables( const Point &startPoint, const Point &endPoint, double bottom, Material *matl,
			      const Point2D &initial, const Point2D &final ) {

  Group * group = new GriddedGroup(10);

  _midX = (endPoint.idx(idx) - startPoint.idx(idx)) / 2.0 + startPoint.idx(idx);
  _midZ = bottom + endPoint.z() / 10.0;

  /*
    dx^2 = 4*a*dz
    
    a = dx^2 / 4*dz
    
   */
 
  double ymk = startPoint.idx(idx) - _midX;
  _a = (ymk*ymk) / (startPoint.z() - _midZ);

  createCable( startPoint, endPoint, matl, group );

  // Now create vertical cables
  addVerticalCables( startPoint.idx(idx), endPoint.idx(idx), bottom, startPoint.idx(oidx), group );


  // Initial: x = far left
  //          y = joint on left side of left tower.
  _midX = initial.x();
  ymk = initial.y() - _midX;
  _a = (ymk*ymk) / (startPoint.z() - _midZ);

  
  if ( idx == 0 ) {
    //std::cerr<< "CREATE CABLE: "<< Point(initial.x(), startPoint.idx(oidx), _midZ)<< " -> " << Point(initial.y(), startPoint.idx(oidx), startPoint.z()) << std::endl;
    //std::cerr << "MIDX: " << _midX << std::endl;

    createCable( Point(initial.x(), startPoint.idx(oidx), _midZ),
		 Point(initial.y(), startPoint.idx(oidx), startPoint.z()), matl, group );
  }
  else {
    //std::cerr<< "CREATE CABLE: "<< Point(startPoint.idx(oidx), initial.x(), _midZ) << " -> "
    //     <<  Point(startPoint.idx(oidx), initial.y(), startPoint.z()) << std::endl;
    //std::cerr << "MIDX: " << _midX << std::endl;

    createCable( Point(startPoint.idx(oidx), initial.x(), _midZ),
		 Point(startPoint.idx(oidx), initial.y(), startPoint.z()), matl, group );
  }
  

  addVerticalCables( initial.x(), initial.y(), bottom, startPoint.idx(oidx), group );

  // Final: x = far right
  //          y = joint on right side of right tower.
  _midX = final.x();
  ymk = final.y() - _midX;
  _a = (ymk*ymk) / (startPoint.z() - _midZ);

  if ( idx == 0 ) {
    createCable( Point(final.x(), endPoint.y(), _midZ),
		 Point(final.y(), endPoint.y(), endPoint.z()), matl, group );
  }
  else {
    createCable( Point(endPoint.idx(oidx), final.x(), _midZ),
		 Point(endPoint.idx(oidx), final.y(), endPoint.z()), matl, group );
  }
  
  addVerticalCables( final.y(), final.x(), bottom, endPoint.idx(oidx), group );
  return group;
}

void
SuspensionBridge::createCable( const Point &p1, const Point &p2, Material * matl, Group *group ) {
  
  // Will subdivide and repeat in future - for now, just create a
  // single cylinder.

  //std::cerr << "CreateCable from " << p1 << " => " << p2 << std::endl;
  Point lineMidpoint = (p2 + p1) * 0.5;
  //std::cerr << "Midpoint is " << lineMidpoint << std::endl;
  Point parabolaMidpoint = lineMidpoint;
  double zmk = parabolaMidpoint.idx(idx) - _midX;
  //std::cerr << "ZMK: " << zmk << std::endl;
  double parabolaHeight = zmk*zmk / _a + _midZ;
  parabolaMidpoint.setIdx(2, parabolaHeight );
  //std::cerr << "Line Midpoint: " << lineMidpoint.z() << " Parabola: " << parabolaMidpoint.z() << std::endl;

  if ( !ABSCLOSE( lineMidpoint.z(), parabolaMidpoint.z(), 0.01 ) ) {
    //std::cerr << "Not close enough: " << (lineMidpoint.z()- parabolaMidpoint.z()) << std::endl;
    //std::cerr << "Cable 1 : " << p1 << " -> " << parabolaMidpoint << std::endl << std::endl;
    createCable( p1, parabolaMidpoint, matl, group );
    //std::cerr << "Cable 2 : " << parabolaMidpoint << " -> " << p2 << std::endl;
    
    createCable( parabolaMidpoint, p2, matl, group );
    return;
  }
  
  //std::cerr << "Creating cable from " << p1 << " -> " << p2 << std::endl;
  Cylinder * cable = new Cylinder( matl, Vector(p2 - p1).length(), _mainCableThickness );
  Position p;

  Matrix4 mat = Matrix4( Matrix3::AlignVectorWithAxis(Vector(p2-p1), Matrix3::Z_AXIS ) ).invert();
  //std::cerr << "MATRIX IS " << mat << std::endl;
  mat.setCol( Vector( p1.x(), p1.y(), p1.z() ), 3 );
  cable->transform(mat);
  group->addObject( cable );
}

Group *
SuspensionBridge::makeTower( const Point2D &minPoint, const Point2D &maxPoint,
		   double midHeight, double top, double bottom) {

  // min x/y, max x/y, middle z, top, bottom.
  Group * group = new Group();

  //std::cerr << "Making support at " << Point( minPoint.x(), minPoint.y(), bottom )
  //<< " -> " << Point( maxPoint.x(), maxPoint.y(), midHeight ) << std::endl;
  Box * support = new Box(_supportMatl,
			  Point( minPoint.x(), minPoint.y(), bottom ),
			  Point( maxPoint.x(), maxPoint.y(), midHeight ) );
  group->addObject(support);

  int numCrosses = 0;

  switch (_towerType) {
  case ARCH_TOP:
    {
      Box * tower = new Box(_towerMatl,
			    Point( minPoint.x(), minPoint.y(), midHeight ),
			    Point( maxPoint.x(), maxPoint.y(), top ) );
      CSG_Primitive * full = new CSG_Primitive(tower);
      // Minus
      //  Small box
      double width = (maxPoint.idx(idx)-minPoint.idx(idx)) * 0.5;
      double sphereRadius = 0.5 * (maxPoint.idx(oidx) - minPoint.idx(oidx) - 2.0*width);
      Point mb1(minPoint,midHeight);
      Point mb2(maxPoint,top-width-sphereRadius);
      mb1.setIdx(idx, minPoint.idx(idx) -0.1);
      mb1.setIdx(oidx, minPoint.idx(oidx) + width );
      mb2.setIdx(idx, maxPoint.idx(idx) + 0.1);
      mb2.setIdx(oidx, maxPoint.idx(oidx) - width );
      
      Box * minusBox = new Box( _towerMatl, mb1, mb2 );
      CSG_Primitive * mbprim = new CSG_Primitive(minusBox);
      CSG_Not * notcsg = new CSG_Not(mbprim);
      
      
      //group->addObject(minusBox);
      //  Sphere
      Point center(0,0,top-width-sphereRadius);
      center.setIdx(idx,0.5*(minPoint.idx(idx)+maxPoint.idx(idx)));
      center.setIdx(oidx,0.5*(minPoint.idx(oidx)+maxPoint.idx(oidx)));
      Sphere * minusSphere = new Sphere(_towerMatl, center, sphereRadius );
      //group->addObject(minusSphere);
      CSG_And * allMinus = new CSG_And( new CSG_Not(new CSG_Primitive(minusSphere)), notcsg );
      CSG_And * andcsg = new CSG_And(allMinus,full);
      CSG * csg = new CSG(andcsg);
      group->addObject(csg);
    }
    break;
  case SQUARE_ONE:
    numCrosses = 1;
  case SQUARE_TWO:
    numCrosses++;
  case SQUARE_TOP:
    
    {
      double width = (maxPoint.idx(idx)-minPoint.idx(idx)) * 0.5;
      
      // TOP
      Box * towerTop = new Box(_towerMatl,
			       Point( minPoint.x(), minPoint.y(), top-width ),
			       Point( maxPoint.x(), maxPoint.y(), top ) );
      //std::cerr << Point( minPoint.x(), minPoint.y(), top ) << std::endl;
      group->addObject(towerTop);
      
      // Supports
      Point p1;
      p1.setIdx(idx, minPoint.idx(idx));
      p1.setIdx(oidx, minPoint.idx(oidx));
      p1.setIdx(2, midHeight);
      Point p2;
      p2.setIdx(idx, maxPoint.idx(idx));
      p2.setIdx(oidx, minPoint.idx(oidx)+width);
      p2.setIdx(2, top);
      group->addObject(new Box(_towerMatl,
			       p1,
			       p2 ));
      
      p1.setIdx(oidx, maxPoint.idx(oidx)-width);
      p2.setIdx(oidx, maxPoint.idx(oidx));
      group->addObject(new Box(_towerMatl,
			       p1,
			       p2));
      if ( numCrosses > 0 ) {
	double heightDiff = (top - midHeight) / (double)(numCrosses+1);

#ifdef ADD_CROSS
	double angle = 60;
	double angleRad = DEGTORAD(angle);
	double ninetyMinus = DEGTORAD(90) - angleRad;
	double top = sideMin.z();
	double bottom = sideMin.z()-railWidth*0.5;
	double thick = width;
	double scaffHeight = (top-bottom)/cos(ninetyMinus) + 2.0*thick / tan(angleRad);
	double dx = 0.5*scaffHeight*cos(angleRad);
	double stride = thick+2.0*dx;
	double sThick = thick / 3.0;
	Material * scafMat = _towerMatl;
	Box * box = new Box(scafMat,
			    Point(-sThick*0.5,
				  -sThick*0.5,
				  0),
			    Point(sThick*0.5,
				  sThick*0.5,
				  scaffHeight));
#endif
	
	for ( int i = 1; i <= numCrosses; ++i ) {
	  group->addObject( new Box(_towerMatl,
				    Point( minPoint.x(), minPoint.y(), midHeight+i*heightDiff ),
				    Point( maxPoint.x(), maxPoint.y(), midHeight+i*heightDiff+width ) ) );
	}
      }
    }
    break;
  case NONE:
    break;
  }

#if 0 //////////////////////////////////////////////////////////////////////
#define TOWER_1
#ifdef TOWER_1

  /* OLD TOWER 1
  Box * tower = new Box(_towerMatl,
			Point( minPoint.x(), minPoint.y(), midHeight ),
			Point( maxPoint.x(), maxPoint.y(), top ) );
  
  group->addObject(tower);
  */

  // Full tower
  Box * tower = new Box(_towerMatl,
			Point( minPoint.x(), minPoint.y(), midHeight ),
			Point( maxPoint.x(), maxPoint.y(), top ) );
  CSG_Primitive * full = new CSG_Primitive(tower);
  // Minus
  //  Small box
  double width = (maxPoint.idx(idx)-minPoint.idx(idx)) * 0.5;
  double sphereRadius = 0.5 * (maxPoint.idx(oidx) - minPoint.idx(oidx) - 2.0*width);
  Point mb1(minPoint,midHeight);
  Point mb2(maxPoint,top-width-sphereRadius);
  mb1.setIdx(idx, minPoint.idx(idx) -0.1);
  mb1.setIdx(oidx, minPoint.idx(oidx) + width );
  mb2.setIdx(idx, maxPoint.idx(idx) + 0.1);
  mb2.setIdx(oidx, maxPoint.idx(oidx) - width );

  Box * minusBox = new Box( _towerMatl, mb1, mb2 );
  CSG_Primitive * mbprim = new CSG_Primitive(minusBox);
  CSG_Not * notcsg = new CSG_Not(mbprim);
  
  
  //group->addObject(minusBox);
  //  Sphere
  Point center(0,0,top-width-sphereRadius);
  center.setIdx(idx,0.5*(minPoint.idx(idx)+maxPoint.idx(idx)));
  center.setIdx(oidx,0.5*(minPoint.idx(oidx)+maxPoint.idx(oidx)));
  Sphere * minusSphere = new Sphere(_towerMatl, center, sphereRadius );
  //group->addObject(minusSphere);
  CSG_And * allMinus = new CSG_And( new CSG_Not(new CSG_Primitive(minusSphere)), notcsg );
  CSG_And * andcsg = new CSG_And(allMinus,full);
  CSG * csg = new CSG(andcsg);
  group->addObject(csg);
#else
  double width = (maxPoint.idx(idx)-minPoint.idx(idx)) * 0.5;

  // TOP
  Box * towerTop = new Box(_towerMatl,
			   Point( minPoint.x(), minPoint.y(), top-width ),
			   Point( maxPoint.x(), maxPoint.y(), top ) );
  //std::cerr << Point( minPoint.x(), minPoint.y(), top ) << std::endl;
  group->addObject(towerTop);

#if 1
  // Supports
  Point p1;
  p1.setIdx(idx, minPoint.idx(idx));
  p1.setIdx(oidx, minPoint.idx(oidx));
  p1.setIdx(2, midHeight);
  Point p2;
  p2.setIdx(idx, maxPoint.idx(idx));
  p2.setIdx(oidx, minPoint.idx(oidx)+width);
  p2.setIdx(2, top);
  group->addObject(new Box(_towerMatl,
			   p1,
			   p2 ));
  
  p1.setIdx(oidx, maxPoint.idx(oidx)-width);
  p2.setIdx(oidx, maxPoint.idx(oidx));
  group->addObject(new Box(_towerMatl,
			   p1,
			   p2));

#endif

#ifdef DIAGONAL
  // Crosspieces
  
  // Width = min-max of idx, height = height / 2
  double cWidth = maxPoint.idx(idx) - minPoint.idx(idx);
  double cHeight = (top-midHeight) * 0.5;
  double crosslen = sqrt( cWidth*cWidth + cHeight*cHeight );

  //Point startPoint(minPoint.idx(idx), minPoint.idx(oidx), midHeight);
  //startPoint.setIdx( idx, startPoint.idx(idx) + crosslen*0.5 );
  Point startPoint( minPoint.x(), minPoint.y(), midHeight );
  startPoint.setIdx( oidx, startPoint.idx(oidx) + crosslen*0.5 );
  std::cerr << "=== IDX " << idx << std::endl;
  std::cerr << startPoint << std::endl;
  Box * b1 = new Box( _towerMatl,
		      startPoint,
		      startPoint + Point(width, width, crosslen) );
  Position p;
  Point pv(0.0);
  pv.setIdx(idx, 45);
  p.rotate( (Vector)pv );
  group->addObject( new Instance(p.getMatrix(b1),
				 b1,
				 _towerMatl) );
  p.translate( Vector(0,0,cHeight), false );
  group->addObject( new Instance(p.getMatrix(b1),
				 b1,
				 _towerMatl) );
  
  p.reset();
  pv.setIdx(idx, -45);
  p.rotate( (Vector)pv );
  group->addObject( new Instance(p.getMatrix(b1),
				 b1,
				 _towerMatl) );

  p.translate( Vector(0,0,cHeight), false );
  group->addObject( new Instance(p.getMatrix(b1),
				 b1,
				 _towerMatl) );
#else // NOT DIAGONAL

  int numCrosses = 1;
#if 0
  if ( drand48() < 0.5 ) {
    numCrosses = 2;
  }
#endif
  
  double heightDiff = (top - midHeight) / (double)(numCrosses+1);

  for ( int i = 1; i <= numCrosses; ++i ) {
    group->addObject( new Box(_towerMatl,
			      Point( minPoint.x(), minPoint.y(), midHeight+i*heightDiff ),
			      Point( maxPoint.x(), maxPoint.y(), midHeight+i*heightDiff+width ) ) );
  }

#endif // DIAGONAL
  
#endif
#endif  //////////////////////////////////////////  
  
  
  return group;
}






ArchBridge::ArchBridge(Material *matl, const Point& minPoint, const Point& maxPoint,
		       double height, double landOrWater) :
  Bridge(matl, minPoint, maxPoint, height, landOrWater )
{
  ALLOC;
  DALLOC(Bridge);
}

ArchBridge::~ArchBridge() {
}


void
ArchBridge::preprocess() {
  Bridge::preprocess();

  makeRoadbed( 1.0 );

  /*
    Arch eqn:

    ax2 + bx + c = z

    at x = 0, z = 0
    at x = width, z = 0
    at x = width/2, z = roadbed

    where roadbed = road - (whatever is zero)
    
    Solve for a,b,c:

    c = 0
    a = -(4*road) / (width*width)
    b = (4*road) / width

  */
  Group * archGroup = new GriddedGroup(10);
  double roadbed = _height-_landOrWater;
  double width = _maxPoint.idx(idx) - _minPoint.idx(idx);
  _b = 4.0 * roadbed / width;
  _a = -_b / width;

  /********** DEBUG 
  std::cerr << "Roadbed: " << roadbed << " Width " << width << std::endl;
  std::cerr << "A is " << _a << " B is " << _b << std::endl;
  std::cerr << getArchHeight( _minPoint.idx(idx) ) << " " << _landOrWater << std::endl;
  std::cerr << getArchHeight( _maxPoint.idx(idx) ) << " " << _landOrWater << std::endl;
  std::cerr << getArchHeight( (_maxPoint.idx(idx) +
  _minPoint.idx(idx)) / 2.0 ) << " " << _height << std::endl;
  ***********/
  
  assert( ABSCLOSE(getArchHeight( _minPoint.idx(idx) ),_landOrWater, 1e-2) );
	  assert( ABSCLOSE(getArchHeight( _maxPoint.idx(idx) ),_landOrWater, 1e-2) );
  assert( ABSCLOSE(getArchHeight( (_maxPoint.idx(idx) + _minPoint.idx(idx)) / 2.0 ), _height, 1e-2) );

  Point p1( _minPoint.x(), _minPoint.y(), _landOrWater );
  Point p2( _maxPoint.x(), _minPoint.y(), _landOrWater );
  if ( idx == 1 ) {
    p2 = Point( _minPoint.x(), _maxPoint.y(), _landOrWater );
  }
  createArch( p1, p2,
	      _supportMatl, archGroup );

  p1 = Point( _minPoint.x(), _maxPoint.y(), _landOrWater );
  p2 = Point( _maxPoint.x(), _maxPoint.y(), _landOrWater );
  if ( idx == 1 ) {
    p1 = Point( _maxPoint.x(), _minPoint.y(), _landOrWater );
  }
  createArch( p1, p2,
	      _supportMatl, archGroup );

  _parts->addObject(archGroup);
  _parts->preprocess();
}

std::string INDENT = "";

double
ArchBridge::getArchHeight( double spot ) {
  // Get offset, since minPoint is set to 0
  double offset = spot - _minPoint.idx(idx);
  assert(offset <= (_maxPoint.idx(idx) - _minPoint.idx(idx)));
  return  _a * offset * offset + _b * offset + _landOrWater; // Since water is of height 0
}

void
ArchBridge::createArch( const Point &p1, const Point &p2, Material * matl, Group *group ) {
  
  //std::cerr << INDENT << "CreateArch from " << p1 << " => " << p2 << std::endl;
  Point lineMidpoint = (p2 + p1) * 0.5;
  //std::cerr << INDENT << "Midpoint is " << lineMidpoint << std::endl;
  Point archMidpoint = lineMidpoint;

  double archHeight = getArchHeight( lineMidpoint.idx(idx) );
  archMidpoint.setIdx(2, archHeight );
  //std::cerr << INDENT << "Arch height is " << archHeight << std::endl;
  
  if ( !ABSCLOSE( lineMidpoint.z(), archMidpoint.z(), 0.0001 ) ) {
    INDENT += " ";
    createArch( p1, archMidpoint, matl, group );
    createArch( archMidpoint, p2, matl, group );
    return;
  }

  //std::cerr << INDENT << "========== SUCCESS " << p1 << "-> " << p2 << " =======" << std::endl;
  //std::cerr << INDENT << "Z Translation is " << p1.z() << std::endl;
  Vector pv = Vector(p2 - p1);
  double pvlen = pv.length();
  //std::cerr << INDENT << " Arch is " << pvlen << std::endl;

  
  pv.normalize();
  Matrix4 mat = Matrix4( Matrix3::AlignVectorWithAxis(pv, Matrix3::Z_AXIS ) ).invert();
  mat.setCol( Vector( p1.x(), p1.y(), p1.z() ), 3 );

  //#define USE_CYLINDER
#ifdef USE_CYLINDER
  Cylinder * arch = new Cylinder( matl, pvlen, 1.0 );
  
  arch->transform(mat);
  
#else
  pvlen *= 1.1;
  double ARCH_THICKNESS = 4.0;
  Point b1(-ARCH_THICKNESS*0.5, -ARCH_THICKNESS*0.5, 0);
  Point b2(ARCH_THICKNESS*0.5, ARCH_THICKNESS*0.5, pvlen);
  Box * archBox = new Box( matl, b1, b2 );
  Instance *arch= new Instance(mat, archBox, matl );
  
#endif
  group->addObject( arch );
  
  
}

