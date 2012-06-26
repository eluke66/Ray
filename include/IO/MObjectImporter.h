#ifndef _M_OBJECT_IMPORTER_H_
#define _M_OBJECT_IMPORTER_H_

#include "IO/ObjectImporter.h"

class MObjectImporter : public ObjectImporter {
public:
  std::string description() {
    return "M object importer";
  }

  ExtensionList getExtensions() {
    ExtensionList extensions;
    extensions.push_back( "m" );
    return extensions;
  }

  virtual Object * importFile( std::string filename );
private:
  MObjectImporter() {
    ObjectRegistry::AddImporter(this);
  }
  static MObjectImporter Importer;

};

#endif
