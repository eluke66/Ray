#include "IO/GTSObjectImporter.h"
#include "Core/utils.h"
#include "Core/Point.h"
#include "Core/Vector.h"
#include "Geometry/Object.h"
#include "Geometry/GriddedGroup.h"
#include "Geometry/Triangle.h"
#include "Core/Color.h"

#include <fstream>

GTSObjectImporter
GTSObjectImporter::Importer;

struct Face {
  Face(){}
  Face(int v1,int v2, int v3 ) : v1(v1), v2(v2), v3(v3) {}
  int v1, v2, v3;
};

struct Edge {
  Edge(){}
  Edge( unsigned p1, unsigned p2 ) : p1(p1), p2(p2) {}
  unsigned p1, p2;
};

Object *
GTSObjectImporter::importFile( std::string filename ) {

  std::ifstream infile(filename.c_str() );
  if ( !infile.is_open() ) {
      std::cerr << "Unable to open GTS file " << filename << std::endl;
      return NULL;
  }
  
  GriddedGroup *geom = new GriddedGroup(20);
  
  std::string input;

  unsigned nVerts, nEdges, nFaces;
  infile >> nVerts >> nEdges >> nFaces;
  
  // Read in vertices and input
  std::vector<Point> vertices;
  std::vector<Face>  faces;
  std::vector<Edge>  edges;

  vertices.resize(nVerts);
  faces.resize(nFaces);
  edges.resize(nEdges);
  
  std::string dummy;
  infile >> dummy >> dummy >> dummy >> dummy;

  if ( !Options::Instance()->quiet() )
    std::cerr << "GTS has " << vertices.size() << " vertices, "
	      << edges.size() << " edges, and "
	      << faces.size() << " faces" << std::endl;
  
  for ( unsigned i = 0; i < nVerts; ++i ) {
    double d1,d2,d3;
    infile >> d1 >> d2 >> d3;
    vertices[i] = Point(d1,d2,d3);
  }

  for ( unsigned i = 0; i < nEdges; ++i ) {
    unsigned e1, e2;
    infile >> e1 >> e2;
    edges[i] = Edge(e1,e2);
  }

  for ( unsigned i = 0; i < nFaces; ++i ) {
    unsigned f1,f2,f3;
    infile >> f1 >> f2 >> f3;
    faces[i] = Face(f1,f2,f3);
  }
  
  // Build triangles
  for ( unsigned i = 0; i < faces.size(); ++i ) {
    Point p1, p2, p3;

    std::map<Point,bool> points;
    points[ vertices[ edges[ faces[i].v1 ].p1 ] ] = true;
    points[ vertices[ edges[ faces[i].v1 ].p2 ] ] = true;
    points[ vertices[ edges[ faces[i].v2 ].p1 ] ] = true;
    points[ vertices[ edges[ faces[i].v2 ].p2 ] ] = true;
    points[ vertices[ edges[ faces[i].v3 ].p1 ] ] = true;
    points[ vertices[ edges[ faces[i].v3 ].p2 ] ] = true;

    std::cerr << "Num unique points: " << points.size() << std::endl;
    for ( std::map<Point,bool>::iterator it = points.begin();
	  it != points.end();
	  ++it ) {
      std::cerr << it->first << std::endl;
    }
#if 0
    p1 = vertices[ edges[ faces[i].v1 ].p1 ];
    p2 = vertices[ edges[ faces[i].v1 ].p2 ];
    p3 = vertices[ edges[ faces[i].v2 ].p1 ];

#endif
    
    Triangle *tri = new Triangle( NULL,
				  p1,
				  p2,
				  p3 );
    
    geom->addObject(tri);
  }
  
  return geom;
}
