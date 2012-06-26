#ifndef _IMAGE_LOADER_H_
#define _IMAGE_LOADER_H_

#include <string>
#include <map>
#include "Core/Image.h"

class ImageImporter;
class ImageExporter;

class ImageLoader {
public:

  // Load the image in the given filename, and return it. Returns
  // null if there is a problem....
  static Image * LoadImage( std::string filename );

  static bool VerifyImage( std::string filename );
private:
  ImageLoader() {}
};

class ImageSaver {
public:

  // Write the image to the given filename. Returns true on success,
  // false on failure.
  static bool SaveImage( Image * image, std::string filename );
private:
  ImageSaver() {}
};

class ImageRegistry {
public:
  static void AddImporter( ImageImporter * importer );
  static ImageImporter * GetImporter( std::string extension );
  
  static void AddExporter( ImageExporter * saver );
  static ImageExporter * GetExporter( std::string extension );
protected:
  typedef std::map<std::string, ImageImporter *> ImportExtensionMap;
  static ImportExtensionMap *Importers;
  typedef std::map<std::string, ImageExporter *> ExportExtensionMap;
  static ExportExtensionMap *Exporters;
  
  ImageRegistry() {}
};


#endif
