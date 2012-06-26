#include <iostream>
#include <fstream>
#include <vector>
#include "Core/Point.h"
#include "Core/utils.h"

struct tri {
  tri(Point a,Point b,Point c) : p1(a),p2(b),p3(c) {}
  Point p1,p2,p3;
};

int 
main(int argc, char ** argv ) {
  assert( argc > 1 );
  std::string filename(argv[1]);
  
  std::ifstream infile(filename.c_str() );
  if ( !infile.is_open() ) {
    std::cerr << "Unable to open gts file " << filename << std::endl;
    return 1;
  }
  
  std::string input;
  char buffer[BUFFER_SIZE];
  infile.getline( buffer, BUFFER_SIZE );

  std::vector<tri> triangles;
  
  while ( infile >> input ) {
    float x1,y1,z1;
    float x2,y2,z2;
    float x3,y3,z3;
    infile >> x1 >> y1 >> z1;
    infile >> x2 >> y2 >> z2;
    infile >> x3 >> y3 >> z3;
    triangles.push_back( tri( Point(x1,y1,z1),
			      Point(x2,y2,z2),
			      Point(x3,y3,z3) ) );
  }
  std::cerr << "We have " << triangles.size() << " triangles!" << std::endl;

  unsigned int i;
  for ( i = 0; i < triangles.size(); ++i ) {
    tri t = triangles[i];
    std::cout << "v " << t.p1.x() << " " << t.p1.y() << " " << t.p1.z() << std::endl;
    std::cout << "v " << t.p2.x() << " " << t.p2.y() << " " << t.p2.z() << std::endl;
    std::cout << "v " << t.p3.x() << " " << t.p3.y() << " " << t.p3.z() << std::endl;
  }

  unsigned which = 0;
  for ( i = 0; i < triangles.size(); ++i, which+=3 ) {
    std::cout << "f " << which << " " << which + 1 << " " << which + 2 << std::endl;
  }
}
