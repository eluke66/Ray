#include "UI/UserInterface.h"
#include "Core/Image.h"
#include "IO/ImageLoader.h"
#include "Jobs/JobManager.h"
#include "Core/Color.h"
#include "Core/Options.h"
#include <iostream>
#include <sstream>


UI::UI(int width, int height ) : _width(width),
				 _height(height),
				 _image(NULL), _cleanup(NULL) {

  // Allocate space for the image
  if ( _image )
    delete _image;

  // If restarting, load that image
  if ( !Options::Instance()->restartFilename().empty() ) {
    _image = ImageLoader::LoadImage( Options::Instance()->restartFilename() );
    if  (!_image)
      std::cerr << "Unable to load restart image!" << std::endl;
  }
  else {
    // Else, create a new one.
    _image = new Image( _width, _height );
  }
  assert( _image );
}


UI::~UI() {
  delete _image;
}

void
UI::dump() {
  saveImage( "part" + Options::Instance()->outputFilename());
}

void 
UI::saveImage(std::string output) {
  // Save the image to a file.
  if ( !ImageSaver::SaveImage( _image, output) ) {
    if ( !ImageSaver::SaveImage( _image, output + ".ppm") ) {
      std::cerr << "Can't save image to file " << output << std::endl;
    }
    else {
      std::cerr << "Saved image as " << (output + ".ppm") << std::endl;
    }
  }
}

void
UI::finish() {
  // Clean up and save our raster image
  if ( _cleanup )
    (*_cleanup)();
}

void
UI::writePixels( int startx, int starty,
		 int endx, int endy ) {
  Color c;

  for ( int i = startx; i < endx; i++ )
    for ( int j = starty; j < endy; j++ )
      {
	c = _image->getPixel(i,j);
	std::cout << (int)(c.R() * 255) << " "
		  << (int)(c.G() * 255) << " "
		  << (int)(c.B() * 255) << " ";
      }
}

void
UI::putPixel( const Color &c, int x, int y ) {
  if ( x < 0 || x >= _width )
    std::cerr << "X is " << x << ", width is " << _width << std::endl;
  assert( x >= 0 && x < _width );
  if ( y < 0 || y >= _height )
    std::cerr << "Y is " << y << ", height is " << _height << std::endl;
  assert( y >=0 && y < _height );

  _image->set( x, y, c);

}

void ThreadedUI::initHelper( int argc, char ** argv ) {
  // Maybe only needed in glut?
}



void ThreadedUI::startJobManager( RaytracerList &raytracers,
				  int width, int height ) {
  assert( _jobManager );

  
  // Set up job manager
  _jobManager->setRaytracers( raytracers );
  _jobManager->setDimensions( width, height );
  
  // Start the manager
  _jobManager->start();
  
}

