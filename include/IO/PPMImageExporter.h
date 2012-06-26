#ifndef _PPMIMAGE_EXPORTER_H_
#define _PPMIMAGE_EXPORTER_H_

#include "IO/ImageExporter.h"

class PPMImageExporter : public ImageExporter {
public:

  virtual ~PPMImageExporter() {}
  
  virtual std::string description() {
    return "PPM Image Writer";
  }

  virtual ImageExporter::ExtensionList getExtensions() {
    ImageExporter::ExtensionList ret;
    ret.push_back( "ppm" );
    ret.push_back( "pnm" );
    ret.push_back( "pgm" );
    return ret;
  }
  
  virtual bool exportFile( Image * image, std::string filename );
  
protected:
  static PPMImageExporter Exporter;
  
  PPMImageExporter() {
    ImageRegistry::AddExporter(this);
  }
};

#endif
