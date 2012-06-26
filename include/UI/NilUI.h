#ifndef _NIL_UI_
#define _NIL_UI_

#include "UI/UserInterface.h"

// This UI class does not display a graphical user
// interface. 
class NilUI : virtual public UI,
	      virtual public ThreadedUI {
 public:

  NilUI( int width, int height );
  virtual ~NilUI();
  
  virtual void initialize( int argc, char ** argv );
  virtual void * start( RaytracerList &raytracers );
  virtual void finish();
  virtual void clear() {}
};

#endif
