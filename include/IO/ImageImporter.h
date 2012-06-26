#ifndef _IMAGE_IMPORTER_H_
#define _IMAGE_IMPORTER_H_

#include "IO/ImageLoader.h"

#include <list>
#include <string>

class ImageImporter {
public:

  virtual ~ImageImporter() {}
  
  virtual std::string description() = 0;
  typedef std::list<std::string> ExtensionList;
  virtual ExtensionList getExtensions() = 0;
  virtual Image * importFile( std::string filename ) = 0;
  
protected:
  ImageImporter() { }
};

#endif
