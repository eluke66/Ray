#ifndef _PPMIMAGE_IMPORTER_H_
#define _PPMIMAGE_IMPORTER_H_

#include "IO/ImageImporter.h"

class PPMImageImporter : public ImageImporter {
public:

  virtual ~PPMImageImporter() {}
  
  virtual std::string description() {
    return "PPM Image Loader";
  }

  virtual ImageImporter::ExtensionList getExtensions() {
    ImageImporter::ExtensionList ret;
    ret.push_back( "ppm" );
    ret.push_back( "pnm" );
    ret.push_back( "pgm" );
    return ret;
  }
  
  virtual Image * importFile( std::string filename );
  
protected:
  static PPMImageImporter Importer;
  
  PPMImageImporter() {
    ImageRegistry::AddImporter(this);
  }
};

#endif
