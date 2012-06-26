#ifndef _GTS_OBJECT_IMPORTER_H_
#define _GTS_OBJECT_IMPORTER_H_

#include "IO/ObjectImporter.h"

class Triangle;

class GTSObjectImporter : public ObjectImporter {
public:
  std::string description() {
    return "GTS object importer";
  }
  
  ExtensionList getExtensions() {
    ExtensionList extensions;
    extensions.push_back( "gts" );
    return extensions;
  }

  virtual Object * importFile( std::string filename );
private:
  GTSObjectImporter() {
    ObjectRegistry::AddImporter(this);
  }
  static GTSObjectImporter Importer;

};

#endif
