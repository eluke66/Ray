#ifndef _3DC_OBJECT_IMPORTER_H_
#define _3DC_OBJECT_IMPORTER_H_

#include "IO/ObjectImporter.h"

class A3DCObjectImporter : public ObjectImporter {
public:
  std::string description() {
    return "3DC object importer";
  }
  
  virtual ObjectImporter::ExtensionList getExtensions() {
    ObjectImporter::ExtensionList extensions;
    extensions.push_back( "3dc" );
    extensions.push_back( "asc" );
    return extensions;
  }

  virtual Object * importFile( std::string filename );
private:
  A3DCObjectImporter() {
    ObjectRegistry::AddImporter(this);
  }
  static A3DCObjectImporter Importer;
};

#endif
