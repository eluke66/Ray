#ifndef _3DS_OBJECT_IMPORTER_H_
#define _3DS_OBJECT_IMPORTER_H_

#include "IO/ObjectImporter.h"
#include <string>
#include <list>
#include <lib3ds/file.h>

class Material;

class _3DSObjectImporter  : public ObjectImporter {
public:
  virtual ~_3DSObjectImporter();
  virtual std::string description() {
    return "3DS Object Importer";
  }

  virtual ExtensionList getExtensions() {
    ExtensionList extensions;
    extensions.push_back("3ds");
    extensions.push_back("3DS");
    return extensions;
  }
  virtual Object * importFile( std::string filename );

protected:
  std::string _type;
  _3DSObjectImporter() {
    ObjectRegistry::AddImporter( this );
  }
  static _3DSObjectImporter Importer;

  
  void getMaterials(Lib3dsFile *f, std::string path, std::map<std::string,Material *> &materials );
};

#endif
