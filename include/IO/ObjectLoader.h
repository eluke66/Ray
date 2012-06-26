#ifndef _OBJECT_LOADER_H_
#define _OBJECT_LOADER_H_

class Object;
class ObjectImporter;
#include <string>
#include <map>

#if 0
class ObjectBuilder {
public:
  static Object * BuildObject( std::istream &input );
protected:

  ObjectBuilder() {}
};
#endif

class ObjectLoader {
public:

  // Load the object in the given filename, and return it. Returns
  // null if there is a problem....
  static Object * LoadObject( std::string filename,
			      std::string partitionerType );
private:
  ObjectLoader(){}
};

class ObjectRegistry {
public:
  static void AddImporter( ObjectImporter * importer );
  static ObjectImporter * GetImporter( std::string extension );
protected:
  typedef std::map<std::string, ObjectImporter *> ExtensionMap;
  static ExtensionMap *Importers;
  ObjectRegistry() {}
};


#endif
