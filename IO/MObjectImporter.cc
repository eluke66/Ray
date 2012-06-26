#include "IO/MObjectImporter.h"
#include "Core/utils.h"
#include "Core/Point.h"
#include "Core/Vector.h"
#include "Geometry/Object.h"
#include "Geometry/GriddedGroup.h"
#include "Geometry/Triangle.h"
#include "Core/Color.h"

#include <fstream>

MObjectImporter
MObjectImporter::Importer;

struct Face {
  Face(int v1,int v2, int v3 ) : v1(v1), v2(v2), v3(v3) {}
  int v1, v2, v3;
};

Object *
MObjectImporter::importFile( std::string filename ) {

  std::ifstream infile(filename.c_str() );
  if ( !infile.is_open() ) {
    std::cerr << "Unable to open M file " << filename << std::endl;
    return NULL;
  }
  
  GriddedGroup *geom = new GriddedGroup(20);
  
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
    if ( input == "vertex" ) {
      int idx;
      float f1, f2, f3;
      infile >> idx >> f1 >> f2 >> f3;
      vertices.push_back( Point(f1,f2,f3) );
    }
    else if ( input == "face" ) {
      int idx, i1,i2,i3;
      infile >> idx >> i1 >> i2 >> i3;
      faces.push_back( Face(i1,i2,i3) );
    }
    else {
      std::cerr << "Unknown op " << input << std::endl;
    }

  }

  if ( !Options::Instance()->quiet() )
    std::cerr << "M file has " << vertices.size() << " vertices, "
              << faces.size() << " faces" << std::endl;


  // Build triangles
  for ( unsigned i = 0; i < faces.size(); ++i ) {
    Triangle *tri = new Triangle( NULL,
                                  vertices[ faces[i].v1 ],
                                  vertices[ faces[i].v2 ],
                                  vertices[ faces[i].v3 ] );
    geom->addObject(tri);
  }

  return geom;
}
