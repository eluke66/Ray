#ifndef _LWO_OBJECT_IMPORTER_H_
#define _LWO_OBJECT_IMPORTER_H_

#include "IO/ObjectImporter.h"

class LWOObjectImporter : public ObjectImporter {
public:
  std::string description() {
    return "LWO object importer";
  }
  
  virtual ObjectImporter::ExtensionList getExtensions() {
    ObjectImporter::ExtensionList extensions;
    extensions.push_back( "lwo" );
    extensions.push_back( "lw" );
    extensions.push_back( "geo" );
    return extensions;
  }

  virtual Object * importFile( std::string filename );
private:
  LWOObjectImporter() {
    ObjectRegistry::AddImporter(this);
  }
  static LWOObjectImporter Importer;
};

#endif
