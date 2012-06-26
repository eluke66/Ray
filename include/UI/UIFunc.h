#ifndef _UIFunc_h_
#define _UIFunc_h_


#include <sstream>
#include "Core/FrameFunc.h"
class UIFunc : public FrameFunc {
public:
  UIFunc( UI *ui ) : _ui(ui)
  {
    assert(_ui);
  }
  virtual void operator()(int whichFrame)
  {
    std::ostringstream ostr;
    ostr << Options::Instance()->outputFilename() << "_";
    int nZeros = numZeros(_nFrames) - numZeros(whichFrame);
    for ( int i = 0; i < nZeros; ++i )
      ostr << "0";
    ostr << whichFrame << ".ppm";
    _ui->saveImage( ostr.str() );
    _ui->clear();
  }
  virtual void doRegister( Scene * )
  { 
  }
private:
  UI *_ui;

  int numZeros( int input )
  {
    int ret = 0;
    while ( input != 0 ) {
      input /= 10;
      ret++;
    }
    return ret;
  }
};

#endif
