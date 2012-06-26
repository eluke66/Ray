#ifndef _SMF_OBJECT_IMPORTER_H_
#define _SMF_OBJECT_IMPORTER_H_

#include "IO/ObjectImporter.h"

class Triangle;

class SMFObjectImporter : public ObjectImporter {
public:
  std::string description() {
    return "SMF object importer";
  }
  
  ExtensionList getExtensions() {
    ExtensionList extensions;
    extensions.push_back( "smf" );
    return extensions;
  }

  virtual Object * importFile( std::string filename );
private:
  SMFObjectImporter() {
    ObjectRegistry::AddImporter(this);
  }
  static SMFObjectImporter Importer;

};

#endif
