#include "IO/ObjectLoader.h"
#include "Core/utils.h"
#include "IO/ObjectImporter.h"
#include "Material/Material.h"
#include "Geometry/Object.h"
#include <sys/stat.h>
#include <sys/types.h>

#ifdef ERIC_FIXME
Compound *
ObjectImporter::makeGeom() {
  Compound * geometry = new Compound;
  assert(geometry);
  geometry->setPartitionerType( SpacePartitionerFactory::StringToType(_type) );
  return geometry;
}


Object *
ObjectBuilder::BuildObject( std::istream &in ) {

  // Here we read in an entry of the type:
  //   {
  //  file "..."
  //  partitioner <type>
  //  surface  { ... additional surface }
  //  position { positional stuff }
  //
  // }
  //
  // positional stuff is:
  //
  // translate <abs/rel> <point> 
  // rotate <vector>
  // scale min/max <value> (scale ss that min/max dimension is that val).
  // scale rel/abs <value> (scale all axes by given value)
  // shear rel/abs <vector> (scale axes by different values).

  char brace;
  std::string input, partitionerType, fileName;
  
  in >> brace;
  
  READLN( input, in );

  Object *object = NULL;
  Surface * currentSurface = NULL;
  Surface nextSurface;
  Position position;
 
  
  while ( input != "}" ) {
    
    if ( input == "file" ) {
      in >> fileName;
    }
    else if ( input == "partitioner" ) {
      in >> partitionerType;
    }
    else if ( input == "surface" ) {

      // Read in the surface
      if ( !currentSurface )
	currentSurface = new Surface;
      in >> nextSurface;
      currentSurface->merge( &nextSurface );
      
    }
    else if ( input == "position" ) {
      in >> position;
    }
    else {
      ERROR("Invalid option to object builder: " + input );
    }
    
    READLN(input, in);
  }

  // Load the object from the file
  assert( !fileName.empty() );
  object = ObjectLoader::LoadObject( fileName, partitionerType );
  
  
  if ( !object ) {
    ERROR( "No valid object specified!" );
    delete currentSurface;
    return NULL;
  }

  DEBUG( "Object bounds before transform: " << object->geometry()->bounds() );
  position.transform( object );
  DEBUG( "Object bounds after transform: " << object->geometry()->bounds() );

  if ( currentSurface )
    if ( Surface::isDefault(object->surface()) ) {
      object->surface( currentSurface );
    }
    else {
      object->surface()->merge( currentSurface );
      delete currentSurface;
    }
  
  return object;
}

#endif

Object *
ObjectLoader::LoadObject( std::string filename, std::string partitionerType ) {
  // Find last occurring '.' in filename
  std::string::size_type position = filename.rfind( '.' );
  std::string extension = filename.substr( position+1 );

  // Gets object importer from ObjectRegistry
  ObjectImporter * importer = ObjectRegistry::GetImporter( extension );

  if ( !importer ) {
    ERROR( "Could not get importer for file " + filename
	   + ", extension " + extension );
    return NULL;
  }
  importer->setPartitioner(partitionerType);
  
  // Try to find the file on disk. In order of preference:
  struct stat buf;
  
  // * Absolute/Relative file name
  if ( stat(filename.c_str(), &buf ) == -1 ) {

    // * In data directory
    char * dataDir = getenv( "RAY_OBJECTS" );
    std::string dataFilename;
    if ( dataDir )
      dataFilename = dataDir + std::string("/") + filename;
    else
      dataFilename = "Objects/" + filename;
    
    if ( stat(dataFilename.c_str(), &buf ) == -1 ) {
      // Failure - notify the user
      ERROR("Unable to find file " << filename);
      return NULL;
    }
    filename = dataFilename;
  }

  // Returns the object from the importer
  return importer->importFile( filename );
}

ObjectRegistry::ExtensionMap *
ObjectRegistry::Importers = NULL;

void 
ObjectRegistry::AddImporter( ObjectImporter * importer ) {
  assert( importer );

  if ( !Importers )
    Importers = new ObjectRegistry::ExtensionMap;
  
  ObjectImporter::ExtensionList extensions = importer->getExtensions();

  for ( ObjectImporter::ExtensionList::const_iterator it = extensions.begin();
	it != extensions.end();
	it++ ) {
    DEBUG( "Adding " + (*it) + " to importer " + importer->description() );
    (*Importers)[ (*it) ] = importer;
  }
  
}

ObjectImporter *
ObjectRegistry::GetImporter( std::string extension ) {
  ExtensionMap::iterator importer = Importers->find( extension );

  if ( importer == Importers->end() ) {
    return NULL;
  }

  return importer->second;
}
