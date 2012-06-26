#include "IO/OffObjectImporter.h"
#include "Core/utils.h"
#include "IO/OffParser.h"
#include "Core/Point.h"
#include "Core/Vector.h"
#include "Geometry/Object.h"
#include "Geometry/GriddedGroup.h"
#include "Geometry/Triangle.h"
#include "Core/Color.h"
#include "Material/PhongMaterial.h"
#include "Core/BoundingBox.h"

OffObjectImporter
OffObjectImporter::Importer;

BoundingBox b;

Object *
OffObjectImporter::importFile( std::string filename ) {

  // Parse the file.
  // Create the geometry.
  // Create the surface.
  // Return the object.
  
  if ( !offPlusRead( filename.c_str() )) {
    ERROR( "Unable to read .off file " << filename);
    return NULL;
  }

  GriddedGroup *geom = new GriddedGroup(20);
  for ( int i = 0; i < PolygonCount; i++ ) {
    
    if ( EdgeCounts[i] == 3 ) {
      Triangle * triangle = processTriangle( i, 0, 1, 2 );

      // Add the triangle to the compound
      geom->addObject(triangle);
    }
    else if ( EdgeCounts[i] == 4 ) {
      // Triangulate them. 
      Triangle * t1 = processTriangle( i, 0, 1, 2 );
      Triangle * t2 = processTriangle( i, 2, 3, 0 );
      geom->addObject( t1 );
      geom->addObject( t2 );
    }
    else {
      ERROR("Polygons of degree " << EdgeCounts[i] << " are not supported!");
    }
    
  } // For each triangle

  std::cerr << "OFF OBJECT BOUNDS: " << b.getMin() << " -> " << b.getMax() << std::endl;
  std::cerr << geom << std::endl;
  return geom;
}
  
Triangle *
OffObjectImporter::processTriangle( int currentPolygon,
				    int i0, int i1, int i2 ) {

  if ( VertexColors ) {
    PRINT_ONCE("Per-vertex colors not yet supported!");
  }
  if ( PolygonColors ) {
    PRINT_ONCE("Per-polygon colors not yet fully supported!");
  }
  // Set the polygon color if specified.
  Color color(0.4, 1, 0.75);
  float kd = 0.3;
  Color specularColor( color );
  int exp = 15;
  
  if ( PolygonColors )
    color = Color( PolygonColors[ currentPolygon ].R,
		   PolygonColors[ currentPolygon ].G,
		   PolygonColors[ currentPolygon ].B );
  
  // Set diffuse color if specified
  if ( DiffuseCoeffFlag )
    kd = DiffuseCoeff;

  // And the specular color if specified
  if ( SpecularCoeffFlag )
    specularColor = Color(SpecularCoeff, SpecularCoeff, SpecularCoeff);

  // And finally, the specular exponent
  if ( SpecularExpFlag )
    exp = (int)SpecularExp;

  Material *matl = new PhongMaterial( color, kd, 0.01,
				      specularColor, exp );

  Triangle *triangle = NULL;
  int polygonIndex = Polygons[currentPolygon][i0];
  Point p1( Vertices[ polygonIndex ].x,
	    Vertices[ polygonIndex ].y,
	    Vertices[ polygonIndex ].z );
  polygonIndex = Polygons[currentPolygon][i1];
  Point p2( Vertices[ polygonIndex ].x,
	    Vertices[ polygonIndex ].y,
	    Vertices[ polygonIndex ].z );
  polygonIndex = Polygons[currentPolygon][i2];
  Point p3( Vertices[ polygonIndex ].x,
	    Vertices[ polygonIndex ].y,
	    Vertices[ polygonIndex ].z );
  
  b.extend(p1); b.extend(p2); b.extend(p3);
  
  if ( VertexOrder == CLOCKWISE_VERTICES )
    triangle = new Triangle( matl, p1, p2, p3 );
  else
    triangle = new Triangle( matl, p3, p2, p1 );

  return triangle;
}
