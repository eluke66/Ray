#ifndef _OFF_OBJECT_IMPORTER_H_
#define _OFF_OBJECT_IMPORTER_H_

#include "IO/ObjectImporter.h"

class Triangle;

class OffObjectImporter : public ObjectImporter {
public:
  std::string description() {
    return "OFF object importer";
  }
  
  ExtensionList getExtensions() {
    ExtensionList extensions;
    extensions.push_back( "aoff" );
    extensions.push_back("off");
    return extensions;
  }

  virtual Object * importFile( std::string filename );
private:
  OffObjectImporter() {
    ObjectRegistry::AddImporter(this);
  }
  static OffObjectImporter Importer;

  Triangle * processTriangle( int currentPolygon,
			      int i0, int i1, int i2 );
};

#endif
