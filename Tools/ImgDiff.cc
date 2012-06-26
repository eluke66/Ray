#include "IO/ImageLoader.h"
#include "Core/Color.h"
#include "Core/Raytracer.h"
#include <iostream>

int LightRaysTrimmed;
void CleanUp() {}
void Raytracer::trace() {}
int
main( int argc, char ** argv ) {

  std::string img1, img2;
  std::string outImg;
  
  assert( argc == 4 );
  
  img1 = argv[1];
  img2 = argv[2];
  outImg = argv[3];

  Image * image1 = ImageLoader::LoadImage( img1 );
  assert( image1 );
  Image * image2 = ImageLoader::LoadImage( img2 );
  assert( image2 );

  assert( image1->width() == image2->width() );
  assert( image1->height() == image2->height() );

  Image * outImage = new Image( image1->width(), image1->height() );

  bool same = true;
  for ( int x = 0 ; x < image1->width(); x++ )
    for ( int y = 0 ; y < image1->height(); y++ ) {
      Color c1 = image1->getPixel( x, y );
      Color c2 = image2->getPixel( x, y );
      Color diff(255*(c2.R() - c1.R()),
		 255*(c2.G() - c1.G()),
		 255*(c2.B() - c1.B()) );

      
      //outImage->putPixel( c1-c2, x, image1->height()-y-1 );
      outImage->set( x, image1->height()-y-1, diff );
      if ( diff.R() != 0.0 || diff.G() != 0.0 || diff.B() != 0.0 ) {
	std::cerr << "Different pixel at " << x << ", " << y << " : " << diff << std::endl;
	same = false;
      }
    }

  std::cerr << "Images are " << (same?"not":"") << " different!" << std::endl;
  assert( ImageSaver::SaveImage( outImage, outImg ) );

  return 0;
}
