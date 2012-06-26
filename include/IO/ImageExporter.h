#ifndef _IMAGE_EXPORTER_H_
#define _IMAGE_EXPORTER_H_

#include "IO/ImageLoader.h"

#include <list>
#include <string>

class Image;

class ImageExporter {
public:

  virtual ~ImageExporter() {}
  
  virtual std::string description() = 0;
  typedef std::list<std::string> ExtensionList;
  virtual ExtensionList getExtensions() = 0;
  virtual bool exportFile( Image * image, std::string filename ) = 0;
  
protected:
  ImageExporter() { }
};

#endif
