#ifndef _UI_H_
#define _UI_H_


class Raytracer;
class Color;

#include <vector>
#include <string>
typedef std::vector<Raytracer *> RaytracerList;

typedef void (*CleanupFunc)();

class Image;
class UI {
  friend class UIFunc;
 public:
  UI( int width, int height );
  virtual ~UI();
  virtual void initialize( int argc, char ** argv ) = 0;
  virtual void * start( RaytracerList &raytracers ) = 0;
  virtual void clear() = 0;
  virtual void finish();

  virtual void putPixel( const Color &c, int x, int y );
  void writePixels( int startx, int starty, int endx, int endy );
  void setCleanupFunction( CleanupFunc func ) { _cleanup = func; }
  void dump();
  
 protected:
  int _width;
  int _height;
  Image *_image;
  CleanupFunc _cleanup;
  
  void saveImage(std::string output);
};

class RemoteJobManager;
class JobManager;
class UIThread;

class ThreadedUI {
  friend class GlutUI;
 protected:
  JobManager * _jobManager;
  RemoteJobManager *_remoteJobManager;
  UIThread   * _myThread;
  
  ThreadedUI() : _jobManager(NULL), _remoteJobManager(NULL), _myThread(NULL) {}

  void initHelper( int argc, char ** argv );
  void startJobManager( RaytracerList &raytracers, int width, int height );
public:
  JobManager *getJobManager() { return _jobManager; }
};


#endif
