#ifndef _JOB_MANAGER_H_
#define _JOB_MANAGER_H_

#include "Jobs/Job.h"
#include "Core/Time.h"
class Raytracer;

#include <vector>
typedef std::vector<Raytracer *> RaytracerList;

class Thread;
class Condition;
class Mutex;
class RemoteJobManager;
class Image;
class FrameFunc;


class JobManager {
 public:
  JobManager();
  virtual ~JobManager();

  void setRaytracers( const RaytracerList &raytracers );
  void setDimensions( int width, int height );
  virtual void start();
  void waitForFinish();
  void addRemoteJobManager( RemoteJobManager &remote );
  
  // Returns true if there's another job in the queue, and if so,
  // copies it into the input variable.
  virtual bool getNextRenderingJob( Job &job );
  void extraTracers(int numTracers) { _extraTracers = numTracers; }
  void restartImage(Image *image) { _restartImage = image; }

  void addFrameFunc( FrameFunc * func ) { _funcs.push_back( func ); }
  void addFrameFuncs( const std::vector<FrameFunc *> &funcs ) {
    _funcs.insert( _funcs.end(), funcs.begin(), funcs.end() );
  }
protected:
  int _w, _h;
  RaytracerList _tracers;
  JobQueue _jobQueue;
  typedef std::vector<Thread *> ThreadList;
  ThreadList _threadList;
  Condition *_jobQueueWaiter, *_frameWaiter;
  Mutex     *_queueWaitMutex, *_queueMutex, *_frameMutex;
  bool       _done;
  int        _extraTracers;
  Image     *_restartImage;
  int        _tracersDone;
  bool       _noMoreJobs;
  typedef std::vector<FrameFunc *> FrameFuncs;
  FrameFuncs _funcs;
  int        _currentFrame;
  Timer      _frameTimer;
  
  void createJobQueue();
  void addJobsToQueue(int xStart, int xSize, int ySize, int N);
  void runRaytracers();
  bool finishFrame();
};

#endif
