#include "Core/Position.h"
#include "Geometry/Box.h"
#include "Core/BoundingBox.h"
#include "Core/utils.h"

Object * mkPoint(bool origin=true) {
  Point center(1,1,1);
  
  if ( origin ) {
    center = Point(0,0,0);
  }

  return new Box( NULL, center, center );
}

Object *mkBox(bool origin = true, int xs=1, int ys=1, int zs=1 ) {
  Point center(1,1,1);
  
  if ( origin ) {
    center = Point(0,0,0);
  }

  return new Box( NULL,
		  center - (Vector)(Point(xs,ys,zs) * 0.5),
		  center + (Vector)(Point(xs,ys,zs) * 0.5) );
		  
}

void test1() {
  Position p;
  BoundingBox b;
  Object *o = mkPoint();
  
  p.translate( Vector(1,1,1), true );
  p.transform( o );
  o->getBounds(b);
  assert( b.getMin().inInterval(Point(1,1,1),1e-6) &&
	  b.getMax().inInterval(Point(1,1,1),1e-6) );
}

void test2() {
  Position p;
  BoundingBox b;
  Object *o = mkPoint();
  
  p.translate( Vector(1,1,1), false );
  p.transform( o );
  o->getBounds(b);
  assert( b.getMin().inInterval(Point(1,1,1),1e-6) &&
	  b.getMax().inInterval(Point(1,1,1),1e-6) );
}

void test1a() {
  Position p;
  BoundingBox b;
  Object *o = mkPoint(false);
  
  p.translate( Vector(1,1,1), true );
  p.transform( o );
  o->getBounds(b);
  assert( b.getMin().inInterval(Point(1,1,1),1e-6) &&
	  b.getMax().inInterval(Point(1,1,1),1e-6) );
}

void test2a() {
  Position p;
  BoundingBox b;
  Object *o = mkPoint(false);
  
  p.translate( Vector(1,1,1), false );
  p.transform( o );
  o->getBounds(b);
  std::cerr << b << std::endl;
  assert( b.getMin().inInterval(Point(2,2,2),1e-6) &&
	  b.getMax().inInterval(Point(2,2,2),1e-6) );
}



void test3() {
  Position p;
  BoundingBox b;
  Object *o = mkBox();
  
  p.translate( Vector(1,1,1), true );
  p.transform( o );
  o->getBounds(b);
  assert( b.getMin().inInterval(Point(0.5,0.5,0.5),1e-6) &&
	  b.getMax().inInterval(Point(1.5,1.5,1.5),1e-6) );
}

void test4() {
  Position p;
  BoundingBox b;
  Object *o = mkBox();
  
  p.translate( Vector(1,1,1), false );
  p.transform( o );
  o->getBounds(b);
  assert( b.getMin().inInterval(Point(0.5,0.5,0.5),1e-6) &&
	  b.getMax().inInterval(Point(1.5,1.5,1.5),1e-6) );
}

void
scaletofit() {
  BoundingBox b( Point(1,1,1), Point(2,2,2) );
  Object *o = mkBox();

  double sca = Position::ScaleToFit( o, b );
  assert( sca  == 1.0 );

  b.extend( Point(5,5,5) );
  sca = Position::ScaleToFit( o, b );
  assert( sca  == 4.0 );

  b.extend( Point(-1, -1, -1) );
  sca = Position::ScaleToFit( o, b );
  assert( sca  == 6.0 );

  b = BoundingBox( Point(1,1,1), Point(1.2, 1.2, 1.2) );
  sca = Position::ScaleToFit( o, b );
  assert( ABSCLOSE(sca, 0.2, 1e-6 ) );

  delete o;
  o = mkBox( true, 2 );
  sca = Position::ScaleToFit( o, b );
  std::cerr << sca << std::endl;
  BoundingBox b1;
  o->getBounds(b1);
  assert( b1.getSize()*sca <= b.getSize() );
}

int
main() {
  std::cerr << "Test1" << std::endl;
  test1();
  std::cerr << "Test2" << std::endl;
  test2();
  std::cerr << "Test1a" << std::endl;
  test1a();
  std::cerr << "Test2a" << std::endl;
  test2a();
  std::cerr << "Test3" << std::endl;
  test3();
  std::cerr << "Test4" << std::endl;
  test4();

  std::cerr << "ScaleToFit" << std::endl;
  scaletofit();
  std::cerr << "Tests okay!" << std::endl;
}




#include "Core/Raytracer.h"
void
Raytracer::trace(){}
void CleanUp() {}
int LightRaysTrimmed;
