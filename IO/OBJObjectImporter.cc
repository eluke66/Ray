#include "IO/OBJObjectImporter.h"
#include "Core/utils.h"
#include "Geometry/Geometry.h"

OBJObjectImporter
OBJObjectImporter::Importer;


Object *
OBJObjectImporter::importFile( std::string filename ) {

  Compound * geometry = makeGeom();
  NOT_DONE( "OBJObjectImporter::importFile" );
  return NULL;
}
