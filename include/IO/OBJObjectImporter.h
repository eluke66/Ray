#ifndef _OBJ_OBJECT_IMPORTER_H_
#define _OBJ_OBJECT_IMPORTER_H_

#include "IO/ObjectImporter.h"

class OBJObjectImporter : public ObjectImporter {
public:
  std::string description() {
    return "OBJ object importer";
  }
  
  virtual ObjectImporter::ExtensionList getExtensions() {
    ObjectImporter::ExtensionList extensions;
    extensions.push_back( "obj" );
    return extensions;
  }

  virtual Object * importFile( std::string filename );
private:
  OBJObjectImporter() {
    ObjectRegistry::AddImporter(this);
  }
  static OBJObjectImporter Importer;
};

#endif
