/*

0.5 = exp(k * 5.6)
  ln(0.5) = k * 5.6
   k = ln(0.5) / 5.6
  
*/


/*

Raleigh scattering:

Blue = 23 % scattered
Green = 10 %
Red = 5 %
*/


#include "Core/utils.h"
#include "Core/Image.h"
#include "IO/ImageLoader.h"
#include <iostream>

using namespace std;

int
main(int argc, char ** argv)
{

  double full = 1.0;
  const double ATMOSPHERE_HEIGHT = 100.0; // Km
  const double EARTH_RADIUS = 6400; // km
  const double TOTAL_DIST = ATMOSPHERE_HEIGHT + EARTH_RADIUS;
  
  const double MAX_HEIGHT = sqrt( TOTAL_DIST*TOTAL_DIST -
				  EARTH_RADIUS*EARTH_RADIUS );
  const double MAX_EXTRA = MAX_HEIGHT - ATMOSPHERE_HEIGHT;
  
  if ( argc < 2 ) {
    cerr << "Usage: " << argv[0] << " <sunAngle in degrees>" << endl;
    return -1;
  }
  double sunAngle = atof(argv[1]);
  
  /* The amount of light that is scattered out
     is proportional to the length of air the sunlight
     has to go through
  */
  
  double atmosphereLength = ATMOSPHERE_HEIGHT +
    MAX_EXTRA*cos(DEGTORAD(sunAngle));
  
  cout << "Max height: " << MAX_HEIGHT << ", max extra: " << MAX_EXTRA
       << ", total length: " << atmosphereLength << endl;
  
  Image img(64, 256);

  double atm2 = atmosphereLength * atmosphereLength;
  const Color SCATTER(1.0, 0.5, 0.25);
  
  for (int y = 0; y < 256; y++ ) {
    Color c;

    double yFrac = (double)y / 256.0;
    double cosScatter = cos( DEGTORAD(90.0*yFrac) );
    
    double factor = (1 + cosScatter*cosScatter);
    c = (SCATTER / atm2) * factor;
    //c.normalize();
    if ( y < 10 || y > 246)
      cerr << "Color: " << c << ", yscatter is " << cosScatter << endl;
    for ( int x = 0; x < 64; ++x )
      img.set(x,y,c);
  }

  
  ImageSaver::SaveImage( &img, "foo.ppm" );

}
