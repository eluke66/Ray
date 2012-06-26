#include "IO/MagicImageImporter.h"
#include "Core/utils.h"
#include "config.h"

MagicImageImporter
MagicImageImporter::Importer;

#include "IO/ImageLoader.h"


namespace IMG {
#ifdef HAVE_WAND_MAGICK_WAND_H
#include <wand/magick-wand.h>
#else
#include <wand/magick_wand.h>
#endif
}


Image * 
MagicImageImporter::importFile( std::string filename ) {

  IMG::MagickWand
    *wand;
  IMG::MagickBooleanType
      status;
  /*
    Read an image.
  */
  IMG::MagickWandGenesis();
  wand=IMG::NewMagickWand();  
  status = IMG::MagickReadImage(wand,filename.c_str());
  if (status == IMG::MagickFalse)
    return NULL;

  unsigned width, height;
  width = IMG::MagickGetImageWidth( wand );
  height = IMG::MagickGetImageHeight( wand );

  float * pixels = new float[ width * height * 3 ];

  status = IMG::MagickGetImagePixels(wand,0,0,width,height,"RGB",IMG::FloatPixel,pixels);

  if (status == IMG::MagickFalse)
    return NULL;
  
  unsigned i = 0;
  Image * image = new Image( width, height );
  for ( unsigned y = 0; y < height; ++y )
    for ( unsigned x = 0; x < width; ++x ) {
      Color c (pixels[i],
	       pixels[i+1],
	       pixels[i+2] );
      image->set( x, y, c );
      i += 3;
    }

  delete[] pixels;
  
  wand=IMG::DestroyMagickWand(wand);
  IMG::MagickWandTerminus();
  return image;
}
