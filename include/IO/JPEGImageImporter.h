#ifndef _JPEGIMAGE_IMPORTER_H_
#define _JPEGIMAGE_IMPORTER_H_

#include "IO/ImageImporter.h"

class JPEGImageImporter : public ImageImporter {
public:

  virtual ~JPEGImageImporter() {}
  
  virtual std::string description() {
    return "JPEG Image Loader";
  }

  virtual ImageImporter::ExtensionList getExtensions() {
    ImageImporter::ExtensionList ret;
    ret.push_back( "jpg" ); ret.push_back( "jpeg" ); ret.push_back("JPG");
    return ret;
  }
  virtual Image * importFile( std::string filename );
  
protected:
  static JPEGImageImporter Importer;
  
  JPEGImageImporter() {
    ImageRegistry::AddImporter( this );
  }
};

#endif
