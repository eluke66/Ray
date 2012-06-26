#ifndef _GLUT_UI_H_
#define _GLUT_UI_H_

#include "UI/UserInterface.h"
#include "Core/Color.h"
#include "Core/Thread.h"
#include <list>

class GlutUI : virtual public UI {
 public:
  GlutUI( int width, int height );
  virtual ~GlutUI();

  virtual void initialize( int argc, char ** argv );
  virtual void * start( RaytracerList &raytracers );
  virtual void finish();
  virtual void clear();
  virtual void putPixel( const Color &c, int x, int y );
  virtual void renderPixel( int x, int y );
  
 protected:
  RaytracerList _tracers;
  bool _rendered;
  
  typedef void (*GlutCallback)();
  void initHelper( int argc, char ** argv,
		   GlutCallback renderCallback,
		   GlutCallback idleCallback = NULL );
  static GlutUI *_myRenderCBObject;
  static GlutUI *_myIdleCBObject;
  
  static void myRenderCallback();
  static void Keyboard( unsigned char key, int, int);
  static void Mouse( int button, int state, int x, int y );



};

class GlutRenderThread;

class ThreadedGlutUI : virtual public GlutUI,
		       virtual public ThreadedUI {
  friend class GlutRenderThread;
 public:
  ThreadedGlutUI( int width, int height );
  virtual ~ThreadedGlutUI();

    
  virtual void initialize( int argc, char ** argv );
  virtual void * start( RaytracerList &raytracers );
  virtual void finish();
  virtual void clear();
  virtual void putPixel( const Color &c, int x, int y );

 protected:
  GlutRenderThread *_renderThread;
  bool _done;
  void drawPixels();

  struct PixelLocation {
    PixelLocation(int x, int y) : x(x), y(y) {}
    int x,y;
  };
  typedef std::list<struct PixelLocation> PixelList;
  PixelList _newPixels;

  Mutex _pixelLock;
};


#endif
