#include "Core/RenderContext.h"
#include "Core/Ray.h"
#include "Core/HitRecord.h"
#include "Geometry/Bump.h"
#include "Geometry/Plane.h"
#include "Core/Image.h"
#include "Core/PerlinNoise.h"
#include "Material/BrickColor.h"
#include "Material/TextureColor.h"

#include "IO/ImageLoader.h"

Vector
RandomBump::getBump( RenderContext &,
		     const Ray &ray,
		     const HitRecord &hit) const
{
      Point p( ray.origin()+hit.minT()*ray.direction() );
#ifdef TEMP_EJ
    double pval = p.x() * p.y() * p.z();
    double d = erand48((unsigned short *)&pval);
    d *= 2*_value;
    d -= _value;
#else
    double d = turbulence( 3, p, M_PI, 1.0 ) * _value;
    
#endif
  return d;
}

Image *
HeightfieldToBumpMap( const Image &in )
{
  assert( in.width() > 1 );
  assert( in.height() > 1 );
  int bmpWidth = in.width() - 1;
  int bmpHeight = in.height() - 1;

  
  Image *bmp = new Image( bmpWidth, bmpHeight );
  assert(bmp);

  // For each square of 4 pixels.
  for ( int x = 0; x < bmpWidth; ++x )
    for ( int y = 0; y < bmpHeight; ++y ) {
      Color c1(in.getPixel(x,y));
      Color c2(in.getPixel(x+1,y));
      Color c3(in.getPixel(x,y+1));
      
      // Get luminance vectors
      // Z is luminance, x and y are vectors
      Vector v1 =
	Point( 1, 0, c2.power() ) -
	Point( 0, 0, c1.power() );
      Vector v2 =
	Point( 0, 1, c3.power() ) -
	Point( 0, 0, c1.power() );

      v1.normalize();
      v2.normalize();

      
      // Get the normal vector
      Vector normal = Cross(v1,v2) - Vector(0,0,1);
#if 0
      std::cerr << "COLORS: " << c1 <<", " << c2 <<"," << c3 << std::endl;
      std::cerr << v1 << ", " << v2 << " => " << normal << std::endl;
#endif
      // Convert the normal vector to a bump color, and store it in
      // the bump map.
      Vector cn = (normal + Vector(1,1,1)) * 0.5;
#if 0
      std::cerr << "\tfinal color is " << cn << std::endl;
#endif
      bmp->set( x, y, Color( cn.x(), cn.y(), cn.z() ) );
    }
  ImageSaver::SaveImage( (Image *)&in, "HF.ppm" );
  ImageSaver::SaveImage( bmp, "BMP.ppm" );
  
  return bmp;

}

Bump * MakeBrickBump( const BrickColor &brick )
{
  // Given a brick color
  BrickColor newBrick( Color(1.0), Color(1.0),
		       brick.aligned(),
		       brick.brickSize(),
		       Color(0.0),
		       brick.uRepeat(), brick.vRepeat() );

  // Create a new heightfield map of the brick
  int imgWidth = std::max(100, int(20 * brick.uRepeat()) );// For now
  int imgHeight = std::max(100, int(20 * brick.vRepeat()) );
  Image hfImage( imgWidth, imgHeight ); 

  Plane plane(NULL,Vector(0,0,1), Point(0,0,0));
  
  RenderContext context(NULL);
  double dx = 1.0 / imgWidth;
  double dy = 1.0 / imgHeight;
  for ( int x = 0; x < imgWidth; ++x )
    for ( int y = 0; y < imgHeight; ++y ) {
      Color result;
      Ray ray( Point( (x+0.5)*dx,
		      (y+0.5)*dy,
		      1 ),
	       Vector( 0, 0, -1 ) );
      HitRecord hit(DBL_MAX);
      hit.setPrimitive( &plane );
      (newBrick)(result, context, ray, hit, Color(1), 0 );
      hfImage.set( x, y, result );
    }
  
  // Convert the image to a bump map
  Image * newImage = HeightfieldToBumpMap( hfImage );
  
  // Return a new MapBump
  return new MapBump( TextureColor( newImage,1,1 )
				    /*brick.uRepeat(),
				      brick.vRepeat() )*/ );
}
