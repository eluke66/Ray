
#include "Core/Image.h"
#include <fstream>
#include <iostream>
using namespace std;

Image::Image(const std::string& filename)
{
  assert(0);
}

Image::Image(int xres, int yres)
  : _data(NULL), _xres(xres), _yres(yres),  _deallocator(NULL)
{
  allocateImage();
}

Image::~Image()
{
  deallocateImage();
}

void Image::write(const std::string& filename) const
{
  assert(0); // Should be going away.
}

void
Image::allocateImage() {

  deallocateImage();

  _data = new Pixel *[_xres];
  for ( int i = 0; i < _xres; i++ ) {
    _data[i] = new Pixel[_yres];
  }  
}

void
Image::deallocateImage() {
  if ( !_data )
    return;

  if ( _deallocator ) {
    (*_deallocator)( _data, _yres );
  }
  else {
    for ( int i = 0; i < _xres; i++ ) {
      delete[] _data[i];
    }
    
    delete[] _data;
  }
  
  _data = NULL;
}

void
Image::setRaster( Pixel ** raster, int width, int height,
                  DeallocateFunction deallocator=NULL ) {
  deallocateImage();
  _data = raster;
  _xres = width;
  _yres = height;
  _deallocator = deallocator;
}
