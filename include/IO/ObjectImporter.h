#ifndef _OBJECT_IMPORTER_H_
#define _OBJECT_IMPORTER_H_

#include "IO/ObjectLoader.h"
#include <string>
#include <list>

class ObjectImporter {
public:

  virtual ~ObjectImporter() {}
  
  virtual std::string description() = 0;
  typedef std::list<std::string> ExtensionList;
  virtual ExtensionList getExtensions() = 0;
  virtual Object * importFile( std::string filename ) = 0;
  void setPartitioner(std::string type) { _type = type; }
protected:
  std::string _type;
  ObjectImporter() {
    //ObjectRegistry::AddImporter( this );
  }
};

#endif
