#include <iostream>
#include <fstream>
#include <vector>
#include "Core/Point.h"
#include "Core/BoundingBox.h"
#include "Core/utils.h"

struct Face {
  Face(int v1,int v2, int v3 ) : v1(v1), v2(v2), v3(v3) {}
  int v1, v2, v3;
};

int 
main(int argc, char ** argv ) {
  assert( argc > 1 );
  std::string filename(argv[1]);
  
  std::ifstream infile(filename.c_str() );
  if ( !infile.is_open() ) {
    std::cerr << "Unable to open SMF file " << filename << std::endl;
    return 1;
  }
  
  std::string input;
  
  // Read in vertices and input
  std::vector<Point> vertices;
  std::vector<Face>  faces;
  
  

  char buffer[BUFFER_SIZE];
  while ( infile >> input ) {
    if ( input[0] == '#' ) {
      infile.getline( buffer, BUFFER_SIZE );
      continue;
    }

    // Convert it to lower case
    toLower(input);

    // Figure out what to do
    if ( input == "v" ) {
      float f1, f2, f3;
      infile >> f1 >> f2 >> f3;
      vertices.push_back( Point(f1,f2,f3) );
    }
    else if ( input == "f" || input == "t") {
      int i1,i2,i3;
      infile >> i1 >> i2 >> i3;
      faces.push_back( Face(i1,i2,i3) );
    }
    else {
      std::cerr << "Unknown op " << input << std::endl;
    }
    
  }
  
  std::cerr << "SMF has " << vertices.size() << " vertices, "
	    << faces.size() << " faces" << std::endl;

  BoundingBox bbox;
  for ( unsigned i = 0; i < vertices.size(); ++i )
    bbox.extend(vertices[i]);
  std::cerr << "Bbox is " << bbox << std::endl;

  /* Printout:
     ntriangles
     bbox min
     bbox max
     t0 t0 t0 t1 t1 t1 t2 t2 t2
     .....
   */
  std::cout << faces.size() << std::endl;
  Point min = bbox.getMin();
  Point max = bbox.getMax();
  std::cout << min.x() << " " << min.y() << " " << min.z() << std::endl;
  std::cout << max.x() << " " << max.y() << " " << max.z() << std::endl;

  for ( unsigned i = 0; i < faces.size(); ++i ) {
    Point p;
    p = vertices[ faces[i].v1 ];
    std::cout << p.x() << " " << p.y() << " " << p.z() << std::endl;
    p = vertices[ faces[i].v2 ];
    std::cout << p.x() << " " << p.y() << " " << p.z() << std::endl;
    p = vertices[ faces[i].v3 ];
    std::cout << p.x() << " " << p.y() << " " << p.z() << std::endl;
  }
}
