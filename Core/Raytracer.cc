#include "Core/Raytracer.h"
#include "Core/Scene.h"
#include "Core/Options.h"
#include "UI/UserInterface.h"

void
Raytracer::trace() {
  _scene->render( _xstart, _ystart,
		  _xend, _yend, _ui );
  if ( Options::Instance()->isSlave() )
    _ui->writePixels( _xstart, _ystart,
		      _xend, _yend );
}
