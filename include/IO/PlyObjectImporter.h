#ifndef _PLY_OBJECT_IMPORTER_H_
#define _PLY_OBJECT_IMPORTER_H_

#include "IO/ObjectImporter.h"

class PlyObjectImporter : public ObjectImporter {
public:
  std::string description() {
    return "PLY object importer";
  }
  
  virtual ObjectImporter::ExtensionList getExtensions() {
    ObjectImporter::ExtensionList extensions;
    extensions.push_back( "ply" );
    return extensions;
  }

  virtual Object * importFile( std::string filename );
private:
  PlyObjectImporter() {
    ObjectRegistry::AddImporter(this);
  }
  static PlyObjectImporter Importer;
};

#endif
