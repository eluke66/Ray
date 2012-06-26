#include "UI/GlutUI.h"
#include "Core/Image.h"
#include "Core/Scene.h"
#include "Jobs/JobManager.h"
#include <GL/glut.h>
#include "UI/UIInterfaceFactory.h"
#include "Jobs/RemoteJobManager.h"
#include "Core/Thread.h"
#include "Core/Raytracer.h"
#include "Core/utils.h"
#include <iostream>
#include "UI/UIFunc.h"

const int REFRESH_TIME = 30; // ms between refreshes

GlutUI *
GlutUI::_myRenderCBObject = NULL;
GlutUI *
GlutUI::_myIdleCBObject = NULL;

GlutUI::GlutUI( int width, int height ) : UI(width, height), _rendered(false) {

}

GlutUI::~GlutUI() {

}


void 
GlutUI::initialize( int argc, char ** argv ) {
  initHelper( argc,
	      argv,
	      myRenderCallback );
}

void * 
GlutUI::start( RaytracerList &raytracers ) {
  _tracers = raytracers;
  assert( _tracers.size() == 1 );

  glutMainLoop();

  return NULL;
}

void
GlutUI::clear() {
  glClear( GL_COLOR_BUFFER_BIT );
}

void 
GlutUI::finish() {
  UI::finish();
}

void 
GlutUI::initHelper( int argc, char ** argv,
		    GlutCallback renderCallback,
		    GlutCallback idleCallback ) {
  
  glutInit(&argc, argv);
  
  glutInitDisplayMode (GLUT_RGB);
  
  /* Create the window */
  glutInitWindowSize ( _width, _height ); 
  glutInitWindowPosition (40, 50);
  glutCreateWindow ("NRT");

  /* Set the clear color */
  glClearColor(0, 0, 0, 0.0);
  
  glViewport (0,
	      0,
	      _width,
	      _height );
  
  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();
  glOrtho (0.0,
	   (GLdouble) _width,
	   0.0,
	   (GLdouble) _height,
	   -1.0,
	   1.0);

  /* Register the event callback functions */
  glutDisplayFunc( renderCallback ); _myRenderCBObject = this;
  if ( idleCallback ) {
    glutIdleFunc( idleCallback ); _myIdleCBObject = this;
  }
  glutKeyboardFunc( Keyboard );
  glutMouseFunc( Mouse );
}

void
GlutUI::putPixel( const Color &c, int x, int y ) {
  
  UI::putPixel(c,x,y);
  
  glColor3f(c.R(), c.G(), c.B());
  glVertex2i(x, y);
}

void 
GlutUI::renderPixel( int x, int y ) {

  if ( _tracers.size() == 0 ) {
    WARN("No tracers available to trace point " << x << ", " << y);
    return;
  }
  Raytracer * raytracer = _tracers.front();

  assert(raytracer);
  mdebugOn;
  raytracer->setWindowRectangle( x, y, x+1, y+1 );
  raytracer->trace();
  mdebugOff;
}

void 
GlutUI::myRenderCallback() {
  assert( _myRenderCBObject );
  
  // Re-render the window

  // If we've already rendered it...
  if ( _myRenderCBObject->_rendered ) {

    // Dump the image to the screen.
    // EJ - may want to optimize this with drawpixels?
    glBegin( GL_POINTS );
    for ( int i = 0; i <  _myRenderCBObject->_width; i++ )
      for ( int j = 0; j < _myRenderCBObject->_height; j++ ) {
	Color c = _myRenderCBObject->_image->getPixel( i, j );
	_myRenderCBObject->putPixel( c, i, j );
      }
    
    // End our rendering
    glEnd();
    glFlush();
  }
  else {

    // We need to render for the first time.
    // Clear the screen
    glClear( GL_COLOR_BUFFER_BIT );
    
    // We're going to do points
    glBegin( GL_POINTS );

    _myRenderCBObject->_tracers.front()->trace();
    
    // End our rendering
    glEnd();
    glFlush();

    // Finish our rendering...
    _myRenderCBObject->finish();
    
    // Denote that we've done the rendering now.
    _myRenderCBObject->_rendered = true;
  }
}

void GlutUI::Keyboard( unsigned char key, int, int) {
  if ( key == 'q' ) {
    ThreadedUI * threadedUI = dynamic_cast<ThreadedUI *>(_myRenderCBObject);
    if ( threadedUI && threadedUI->_remoteJobManager ) {
      threadedUI->_remoteJobManager->abort();
    }
    exit(0);
  }
}

void GlutUI::Mouse(int button, int state, int x, int y) 
{
  if(state == GLUT_DOWN) 
    {
      switch(button) 
        {
	case GLUT_LEFT_BUTTON:
	  std::cerr << "[" << x << ", " << y << "]" << std::endl;
	  break;
        }
    }
  else {
    mdebugOn;
    std::cerr << "Rendering pixel: " << _myRenderCBObject << std::endl;
    _myRenderCBObject->renderPixel(x,y);
    mdebugOff;
  }
}

// Register us with the UI
struct GlutUIInit { GlutUIInit() { UIInterfaceFactory::AddUICreator( new UICreatorImpl<GlutUI>(), "glut" ); } }; static GlutUIInit _theGlutUIInit;


class GlutRenderThread : public Thread {
 public:
  GlutRenderThread( int argc, char ** argv,
		    int width, int height,
		    ThreadedGlutUI &ui );

  virtual void *run();
 protected:
  int _argc;
  char ** _argv;
  int _width, _height;
  ThreadedGlutUI &_ui;
  static void RenderCB();
  static void DumpPixels(int value);
  static ThreadedGlutUI *globalUI;
};


ThreadedGlutUI::ThreadedGlutUI( int width, int height ) :
  UI(width, height),
  GlutUI(width,height),
  ThreadedUI(),
  _renderThread(NULL),
  _done(false),
  _pixelLock("New Pixel Lock")
{

}

  
ThreadedGlutUI::~ThreadedGlutUI() {

}

void 
ThreadedGlutUI::initialize( int argc, char ** argv ) {
  
  // Create Job Manager
  _jobManager = new JobManager();
  _jobManager->addFrameFunc( new UIFunc( this ) );

  // Create a new ui rendering thread
  _renderThread = new GlutRenderThread( argc, argv,
					_width, _height,
					*this );
}

void * 
ThreadedGlutUI::start( RaytracerList &raytracers ) {

  
  const Options::SlaveList &slaves = Options::Instance()->slaves();
  
  _jobManager->extraTracers( slaves.size() );

  if ( !Options::Instance()->restartFilename().empty() )
    _jobManager->restartImage( _image );
  
  // Start the job manager
  startJobManager( raytracers, _width, _height );

  // Set up any remote jobs if needed
  if ( slaves.size() > 0 ) {
    _remoteJobManager =
      new RemoteJobManager( slaves, *this, *_jobManager );
    _remoteJobManager->start();
  }
  _tracers = raytracers;
  
  // Start our rendering thread
  _renderThread->start( false );

  return NULL;
}

void
ThreadedGlutUI::clear()
{
  // No-op for now...
}

void 
ThreadedGlutUI::finish() {
  
  // Wait for job manager to finish
  _jobManager->waitForFinish();

  if ( _remoteJobManager ) {
    _remoteJobManager->waitForFinish();
  }
  
  // Finish rest of UI tasks
  UI::finish();

  _done = true;
  
  // And wait for our ui rendering thread to finish.
  _renderThread->join();
}

void
ThreadedGlutUI::drawPixels() {

  // Dump the image to the screen.
  // EJ - may want to optimize this with drawpixels?
  glBegin( GL_POINTS );

  int damaged = glutLayerGet(GLUT_NORMAL_DAMAGED);
  if ( damaged ) {
    for ( int i = 0; i <  _width; i++ )
      for ( int j = 0; j < _height; j++ ) {
	Color  c = _image->getPixel( i, j );
	glColor3f(c.R(), c.G(), c.B());
	glVertex2i(i, j);
      }
  }
  else {
    MutexLocker lock( _pixelLock );
    PixelList::iterator it;
    for ( it = _newPixels.begin(); it != _newPixels.end(); it++ ) {
      Color c = _image->getPixel( it->x, it->y );
      glColor3f( c.R(), c.G(), c.B() );
      glVertex2i( it->x, it->y );
    }
    
    _newPixels.clear();
  }
  
  // End our rendering
  glEnd();
  glFlush();
};

// Register us with the UI
struct ThreadedGlutUIInit { ThreadedGlutUIInit() { UIInterfaceFactory::AddUICreator( new UICreatorImpl<ThreadedGlutUI>(), "glut_threaded" ); } };
static ThreadedGlutUIInit _theThreadedGlutUIInit;

ThreadedGlutUI *
GlutRenderThread::globalUI = NULL;

GlutRenderThread::GlutRenderThread( int argc, char ** argv,
				    int width, int height,
				    ThreadedGlutUI &ui ) :
  Thread("Glut Render Thread"),
  _argc(argc),_argv(argv),
  _width(width), _height(height),
  _ui(ui) {
    
   
    globalUI = &_ui;
  }

void
GlutRenderThread::DumpPixels(int value) {
  glutPostRedisplay();
  
  if ( !globalUI->_done )
    // Reset up a timer to render the screen
    glutTimerFunc( REFRESH_TIME,
		   DumpPixels,
		   0 );
}

void *
GlutRenderThread::run() {
  
  _ui.GlutUI::initHelper( _argc, _argv,
			  RenderCB );

  
  while (1) {

    // Set up a timer to render the screen
    glutTimerFunc( REFRESH_TIME,
		   DumpPixels,
		   0 );
    
    // Start the glut rendering
    glutMainLoop();
  }

  return NULL;
}


void
GlutRenderThread::RenderCB() {

  globalUI->drawPixels();
}

void
ThreadedGlutUI::putPixel( const Color &c, int x, int y ) {

  MutexLocker lock( _pixelLock );

  _newPixels.push_back( PixelLocation(x,y) );
  UI::putPixel(c,x,y);
}
