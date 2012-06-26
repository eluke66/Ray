#include <iostream>
#include "Core/BoundingBox.h"

int
main() {
  BoundingBox b;
  double x,y,z;
  unsigned numVertices, dummy;
  std::cin >>numVertices >> dummy;
  std::cerr << "processing " << numVertices << " vertices" << std::endl;
  for ( unsigned i = 0; i < numVertices; ++i ) {
    std::cin >> x >> y >> z;
    b.extend(Point(x,y,z));
    if ( ! i % (numVertices/100 ) )
      std::cerr << "." << std::endl;
  }

  std::cout << b << std::endl;
}
