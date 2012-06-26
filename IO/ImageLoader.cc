#include "IO/ImageLoader.h"
#include "Core/utils.h"
#include <iostream>
#include "IO/ImageImporter.h"
#include "IO/ImageExporter.h"
#include <sys/stat.h>
#include <sys/types.h>
#include "config.h"

bool
ImageLoader::VerifyImage( std::string filename ) {
  Image * img = LoadImage( filename );
  bool retVal = img;
  delete img;

  return retVal;
}


Image *
ImageLoader::LoadImage( std::string filename ) {
  // Find last occurring '.' in filename
  std::string::size_type position = filename.rfind( '.' );
  std::string extension = filename.substr( position+1 );

  // Gets image importer from ImageRegistry
  ImageImporter * importer = ImageRegistry::GetImporter( extension );

  if ( !importer ) {
    ERROR( "Could not get importer for file " + filename
	   + ", extension " + extension );
    return NULL;
  }

  // Try to find the file on disk. In order of preference:
  struct stat buf;
  
  // * Absolute/Relative file name
  if ( stat(filename.c_str(), &buf ) == -1 ) {

    // * In data directory
    char * dataDir = getenv( "RAY_IMAGES" );
    std::string dataFilename;
    if ( dataDir )
      dataFilename = dataDir + std::string("/") + filename;
    else
      dataFilename = "Images/" + filename;

    if ( stat(dataFilename.c_str(), &buf ) == -1 ) {
      // Failure - notify the user
      ERROR("Unable to find file " << filename);
      return NULL;
    }
    filename = dataFilename;
  }
  
  // Returns the image from the importer
  return importer->importFile( filename );
}

ImageRegistry::ImportExtensionMap *
ImageRegistry::Importers = NULL;

void 
ImageRegistry::AddImporter( ImageImporter * importer ) {
  assert( importer );

  if (!Importers)
    Importers = new ImageRegistry::ImportExtensionMap;
  
  ImageImporter::ExtensionList extensions = importer->getExtensions();

  for ( ImageImporter::ExtensionList::const_iterator it = extensions.begin();
	it != extensions.end();
	it++ ) {
    DEBUG( "Adding " + (*it) + " to importer " + importer->description() );
    (*Importers)[ (*it) ] = importer;
  }
  
}

ImageImporter *
ImageRegistry::GetImporter( std::string extension ) {
#ifdef USE_MAGICK_WAND
  ImportExtensionMap::iterator importer = Importers->find( "*" );
#else
  ImportExtensionMap::iterator importer = Importers->find( extension );
#endif
  
  if ( importer == Importers->end() ) {
    return NULL;
  }

  return importer->second;
}



////////////////////////////////////////////////////
//
// Code for savers
//

bool
ImageSaver::SaveImage( Image * image, std::string filename ) {
  // Find last occurring '.' in filename
  std::string::size_type position = filename.rfind( '.' );
  std::string extension = filename.substr( position+1 );

  // Get the image exporter from ImageRegistry
  ImageExporter * exporter = ImageRegistry::GetExporter( extension );

  if ( !exporter ) {
    ERROR( "Could not get exporter for file " + filename
	   + ", extension " + extension );
    return false;
  }
  
  // Returns the result from the exporter.
  return exporter->exportFile( image, filename );
}

ImageRegistry::ExportExtensionMap *
ImageRegistry::Exporters = NULL;

void 
ImageRegistry::AddExporter( ImageExporter * exporter ) {
  assert( exporter );

  if (!Exporters)
    Exporters = new ImageRegistry::ExportExtensionMap;
  
  ImageExporter::ExtensionList extensions = exporter->getExtensions();

  for ( ImageExporter::ExtensionList::const_iterator it = extensions.begin();
	it != extensions.end();
	it++ ) {
    DEBUG( "Adding " + (*it) + " to exporter " + exporter->description() );
    (*Exporters)[ (*it) ] = exporter;
  }
  
}

ImageExporter *
ImageRegistry::GetExporter( std::string extension ) {
  ExportExtensionMap::iterator exporter = Exporters->find( extension );

  if ( exporter == Exporters->end() ) {
    return NULL;
  }

  return exporter->second;
}

