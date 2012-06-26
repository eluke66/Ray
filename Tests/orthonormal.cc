#include "Core/Vector.h"
#include "Core/utils.h"
#include <iostream>

void testVector( const Vector &v,bool printPass=true ) {
  Vector ov = v.orthonormal();
  ov.normalize();

  double d = Dot(v,ov);
  if ( !ABSCLOSE(d,0,1e-6) ) {
    std::cerr << "****FAIL: " << v << " (" << ov << ")" << std::endl;
  }
  else
    if ( printPass )
      std::cerr << "PASS: " << v << " (" << ov << ")" << std::endl;
}

int
main() {
  Vector v1(0,0,1);
  testVector(v1);

  Vector v2(0,1,0);
  testVector(v2);
  Vector v3(0,1,1);
  testVector(v3);
  Vector v4(1,0,0);
  testVector(v4);
  Vector v5(1,0,1);
  testVector(v5);
  Vector v6(1,1,0);
  testVector(v6);

  for ( unsigned i = 0; i <10000000; ++i ) {
    Vector v = Vector::Rand();
    v.normalize();
    testVector(v,false);
  }
  
}

