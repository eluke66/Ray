#include <iostream>
#include "Core/BoundingBox.h"

struct triangle {
  triangle() {}
  triangle(int i1, int i2, int i3 ) :
    i1(i1), i2(i2), i3(i3) {}
  int i1,i2,i3;
};

int
main() {
  BoundingBox b;
  double x,y,z;
  unsigned numVertices, numFaces;
  std::cin >>numVertices >> numFaces;
  std::cerr << "processing " << numVertices << " vertices" << std::endl;
  Point *parr = new Point[ numVertices ];
  
  for ( unsigned i = 0; i < numVertices; ++i ) {
    std::cin >> x >> y >> z;
    parr[i] = Point(x,y,z);
    b.extend(Point(x,y,z));
  }

  std::cerr<< "processing " << numFaces << " faces" << std::endl;
  triangle *tarr = new triangle[ numFaces ];
  int i1,i2,i3;
  for (unsigned i = 0; i < numFaces; ++i ) {
    std::cin >>i1 >> i2 >> i3;
    tarr[ i ] = triangle(i1,i2,i3);
  }

  std::cerr << "Outputting..." << std::endl;
  std::cout << numFaces << std::endl;
  std::cout << b.getMin().x() << " "
	    << b.getMin().y() << " "
	    << b.getMin().z() << " ";
  std::cout << b.getMax().x() << " "
	    << b.getMax().y() << " "
	    << b.getMax().z() << std::endl;
  
  for (unsigned i = 0; i < numFaces; ++i ) {
    Point p1,p2,p3;
    p1 = parr[ tarr[i].i1 ];
    p2 = parr[ tarr[i].i2 ];
    p3 = parr[ tarr[i].i3 ];
    
    std::cout << p1.x() << " " << p1.y() << " " << p1.z()
	      << p2.x() << " " << p2.y() << " " << p2.z()
	      << p3.x() << " " << p3.y() << " " << p3.z()
	      << std::endl;
  }

}
