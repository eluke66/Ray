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
  double x,y,z;
  unsigned numVertices, numFaces;
  std::cin >>numVertices >> numFaces;
  std::cerr << "processing " << numVertices << " vertices" << std::endl;
  
  for ( unsigned i = 0; i < numVertices; ++i ) {
    std::cin >> x >> y >> z;
    std::cout << "v " << x << " " << y << " " << z << std::endl;
  }

  std::cerr<< "processing " << numFaces << " faces" << std::endl;
  int i1,i2,i3;
  for (unsigned i = 0; i < numFaces; ++i ) {
    std::cin >>i1 >> i2 >> i3;
    std::cout << "f " << (i1+1) << " " << (i2+1) << " " << (i3+1) << std::endl;
  }
}
