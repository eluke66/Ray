#include "IO/LWOObjectImporter.h"
#include "Core/utils.h"
#include "IO/lw.h"
#include "Geometry/Object.h"
#include "Material/PhongMaterial.h"
#include "Geometry/GriddedGroup.h"
#include "Geometry/Triangle.h"
#include "Core/Options.h"

LWOObjectImporter
LWOObjectImporter::Importer;

Object *
readNode_LWO1(const std::string& fileName);

Object *
LWOObjectImporter::importFile( std::string filename ) {

  Object * returnValue =  readNode_LWO1( filename );
  if ( returnValue )
    return returnValue;

  else
    return NULL;
    
}

Object *
readNode_LWO1(const std::string& fileName)
{
  lwObject* lw = lw_object_read(fileName.c_str(),std::cerr);
  if (!lw)
    return NULL;

  if ( !Options::Instance()->quiet() ) {
    std::cerr << "faces " << lw->face_cnt << std::endl;
    std::cerr << "materials " << lw->material_cnt << std::endl;
    std::cerr << "vertices " << lw->vertex_cnt << std::endl;
  }

  GriddedGroup * geom = new GriddedGroup(20);

  std::vector<Material *> materials;
  for ( int i = 0; i < lw->material_cnt; ++i ) {
    lwMaterial &mat = lw->material[ i ];
    materials.push_back( new PhongMaterial( Color( mat.r,mat.g,mat.b ),
					    0.3, 0.01,
					    Color( mat.r,mat.g,mat.b ), 99 ) );
    
  }
  
  // For each face:
  for ( int i = 0; i < lw->face_cnt; i++ ) {
    lwFace * face = &lw->face[i];
    if ( face->index_cnt < 3 ) {
      PRINT_ONCE( "Degenerate faces in lwo object" );
    }
    else if ( face->index_cnt > 3 )
    { 
      // Triangulate the face, using vertices [0,1,2], [0,2,3],
      // [0,3,4], etc.
      Point p0( lw->vertex[face->index[0]*3+0],
		lw->vertex[face->index[0]*3+1],
		lw->vertex[face->index[0]*3+2] );
      
      for ( int whichVertex = 1; whichVertex < lw->face[i].index_cnt-1; whichVertex++ ) {
	Point p2( lw->vertex[face->index[whichVertex]*3+0],
		  lw->vertex[face->index[whichVertex]*3+1],
		  lw->vertex[face->index[whichVertex]*3+2] );
	Point p3( lw->vertex[face->index[whichVertex+1]*3+0],
		  lw->vertex[face->index[whichVertex+1]*3+1],
		  lw->vertex[face->index[whichVertex+1]*3+2] );
	Triangle * triangle = new Triangle( materials[ face->material ],
					    p0, p2, p3 );
	geom->addObject( triangle );
      }
    }
    else if (face->index_cnt == 3) {
      Point p1( lw->vertex[face->index[0]*3+0],
		lw->vertex[face->index[0]*3+1],
		lw->vertex[face->index[0]*3+2] );
      Point p2( lw->vertex[face->index[1]*3+0],
		lw->vertex[face->index[1]*3+1],
		lw->vertex[face->index[1]*3+2] );
      Point p3( lw->vertex[face->index[2]*3+0],
		lw->vertex[face->index[2]*3+1],
		lw->vertex[face->index[2]*3+2] );
      
      Triangle * triangle = new Triangle( materials[ face->material ], p1, p2, p3 );
      
      geom->addObject( triangle );
    }
    
  } // for each face.

  return geom;

}
