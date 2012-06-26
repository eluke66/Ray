#ifndef _MagicIMAGE_IMPORTER_H_
#define _MagicIMAGE_IMPORTER_H_

#include "IO/ImageImporter.h"

class MagicImageImporter : public ImageImporter {
public:

  virtual ~MagicImageImporter() {}
  
  virtual std::string description() {
    return "Magic Image Loader";
  }

  virtual ImageImporter::ExtensionList getExtensions() {
    ImageImporter::ExtensionList ret;
    ret.push_back( "*" );
    return ret;
  }
  
  virtual Image * importFile( std::string filename );
  
protected:
  static MagicImageImporter Importer;
  
  MagicImageImporter() {
    ImageRegistry::AddImporter(this);
  }
};

#endif
