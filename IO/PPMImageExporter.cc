#include "IO/PPMImageExporter.h"
#include "Core/utils.h"
#include "stdio.h"

PPMImageExporter
PPMImageExporter::Exporter;


bool
PPMImageExporter::exportFile( Image * image, std::string filename ) {

  int x, y;
  unsigned char r, g, b;
  FILE *out;
  int width, height;
  
  assert( image );
  out = fopen(filename.c_str(), "w");
  if (!out) {
    std::cerr << "Can't open output file " << filename << std::endl;
    return false;
  }

  width = image->width();
  height = image->height();
  
  fprintf(out, "P6\n");   // color rawbits format
  fprintf(out, "%d %d\n%d\n",
	  width,
	  height,
	  255);  // width, height, and depth

  Pixel ** raster = image->getRaster();
  assert( raster );
  
  for (y = 0; y < height; y++) {
    for (x = 0; x < width; x++) {
      r = raster[x][height - y - 1].r;  
      g = raster[x][height - y - 1].g;
      b = raster[x][height - y - 1].b;
      fprintf(out, "%c%c%c", r, g, b);
    }
  }
  fprintf(out, "\n");
  fclose(out);

  return true;
}
