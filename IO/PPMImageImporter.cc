#include "IO/PPMImageImporter.h"
#include "Core/utils.h"

extern "C" {
#include <pgm.h>
#include <ppm.h>
}

PPMImageImporter
PPMImageImporter::Importer;

void
freeppmraster( Pixel ** raster, int rows ) {
  ppm_freearray( (char **)raster, rows );
}

#include "IO/ImageLoader.h"

Image * 
PPMImageImporter::importFile( std::string filename ) {

  pixel ** foo = NULL;
  FILE * f = fopen(filename.c_str(), "r");
  pixval maxval;
  int width, height;

  assert(f);
  foo = ppm_readppm(f, &width, &height, &maxval);

  Image * returnImage = new Image(width,height);

  for ( int x = 0; x < width; ++x )
    for ( int y = 0; y < height; ++y ) {
      pixel p1 = foo[height - y - 1][x];
      Pixel p; p.r = p1.r ; p.g = p1.g; p.b = p1.b ; 
      returnImage->set(x,y,p);
    }
  //  returnImage->setRaster( foo, width, height, freeppmraster );

  ImageSaver::SaveImage(returnImage, "delme.ppm" );
  
  return returnImage;
}
