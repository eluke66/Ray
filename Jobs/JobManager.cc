#include "Jobs/JobManager.h"
#include "Core/Thread.h"
#include "Core/Scene.h"
#include "Core/Raytracer.h"
#include "Core/Options.h"
#include "UI/UserInterface.h"
#include "Jobs/JobBuilder.h"
#include "Core/FrameFunc.h"
#include <iostream>
#include <sstream>

class TracerThread : public Thread {
public:
  TracerThread( Raytracer * tracer,
		JobManager &manager) : Thread("TracerThread"),
					_tracer(tracer),
					_manager(manager) {
    assert( _tracer );
  }

  virtual void *run() {
    Job job;

    // Run forever
    while ( 1 ) {

      // Get a job from the job manager.
      if ( _manager.getNextRenderingJob( job ) ) {

	// Set the rendering window for the raytracer
	_tracer->setWindowRectangle( job.startx, job.starty,
				     job.endx,   job.endy );

	// Trace the rays.
	_tracer->trace();
      }
      else {
	// If there are none left, we're done.
	return NULL;
      }

      
    } // End while 
  }
    
protected:
  Raytracer *_tracer;
  JobManager &_manager;
};

std::ostream& operator<<( std::ostream &out, const Job& job ) {
  out << job.startx << ", " << job.starty << " -> " << job.endx << ", " << job.endy;
  return out;
}

std::istream & operator>>(std::istream &s, Job &job) {
  char dummy[10];
  s >> job.startx >> dummy >> job.starty >> dummy >> job.endx >> dummy >> job.endy;
  return s;
}

JobManager::JobManager() : _w(-1), _h(-1), _done(false), _extraTracers(0),
			   _restartImage(NULL), _currentFrame(0) {
}
  
JobManager::~JobManager(){
}

void 
JobManager::setRaytracers( const RaytracerList &raytracers ){
  _tracers = raytracers;
}

void 
JobManager::setDimensions( int width, int height ){
  _w = width;
  _h = height;
}

void 
JobManager::start() {
  assert( _w > 0 );
  assert( _h > 0 );
  assert( _tracers.size() > 0 );

  int numFrames = Options::Instance()->numFrames();
  for ( FrameFuncs::iterator it = _funcs.begin();
	it != _funcs.end();
	++it ) {
    (*it)->setNumFrames( numFrames );
  }
  // Create the list of jobs:
  createJobQueue();

  _frameTimer.restart();
  
  // Setup and run raytracers.
  runRaytracers();
}

void JobManager::runRaytracers() {

  // Set up threading and mutexes.
  _queueWaitMutex = new Mutex( "Job Manager Queue Wait Mutex" );
  _queueMutex     = new Mutex( "Job Manager Queue Mutex" );
  _frameMutex     = new Mutex( "Job Manager Frame Mutex" );
  _jobQueueWaiter = new Condition( *_queueWaitMutex );
  _frameWaiter    = new Condition( *_frameMutex );
  _threadList.resize( _tracers.size() );
  
  // For each tracer in the list of raytracers:
  for ( unsigned i = 0; i < _tracers.size(); i++ ) {
    
    // Create a tracerthread with the raytracer.
    TracerThread * thread = new TracerThread( _tracers[i], *this );

    _threadList[i] = thread;
    
    // Start the thread
    thread->start(false);
  }
}


void 
JobManager::waitForFinish() {

  if ( !_done ) {
    // Wait until the job queue is empty
    _jobQueueWaiter->wait();

  }

  // Join all of the rendering threads
  for ( unsigned i = 0; i < _threadList.size(); i++ ) {
    _threadList[i]->join();
  }
}
extern Image * RestartImage;
void JobManager::createJobQueue() {

#ifdef OLD_QUEUE
  // We create the job queue based on the number of renderers (N) and
  // the size of the rendering window S(x,y). The first N jobs are of
  // size S(x/2, y/2N). This uses the first half of the screen.
  int N = _tracers.size() * 2;
  int xSize = _w / 2;
  int ySize = _h / (2 * N);
  addJobsToQueue( 0, xSize, ySize, N );
  

  // The second N jobs are of size S(x/4, y/2N). This uses the third
  // quarter of the screen.
  xSize = _w / 4;
  addJobsToQueue( _w / 2, xSize, ySize, N );

  // The final 2N jobs are of size S(x/8, y/2N). This uses two of the
  // last eighths of the screen.
  xSize = _w / 8;
  addJobsToQueue( 3*_w / 4, xSize, ySize, N );
  addJobsToQueue( 7*_w / 8, xSize, ySize, N );
#else
  _tracersDone = _tracers.size() + _extraTracers;
  int numJobs = (_tracers.size() + _extraTracers) * 24;

  //JobBuilder::MakeJobsStripes( _jobQueue, _w, _h, numJobs );
  //JobBuilder::MakeJobsTiles( _jobQueue, _w, _h, numJobs );

  if  (_restartImage) {
    std::cerr << "Restarting image" << std::endl;
    JobBuilder::MakeJobsFromImage( _jobQueue, _restartImage, numJobs );
  }
  else
    JobBuilder::MakeJobsStripes( _jobQueue, _w, _h, numJobs );
#endif
  
}

void JobManager::addJobsToQueue(int xStart, int xSize, int ySize, int N) {
  int i;
  
  // Add all but the last job for this size to the queue.
  for ( i = 0; i < N-1; i++ ) {
    Job j(xStart, i*ySize, xSize+xStart, (i+2)*ySize);
    // DEBUG std::cerr << "Adding job " << j << std::endl;
    _jobQueue.push( j );
  }

  // Get the last pixel(s) if we don't get a clean divide.
  ySize = _h - (N-1)*ySize*2;
  int yEnd = (i+1)*ySize-1;
  // (Ensure that we don't go over the top)
  if ( yEnd >= _h )
    yEnd = _h;
  
  Job lastJob(xStart, (N-1)*ySize, xSize+xStart, yEnd);
  // DEBUG std::cerr << "Adding last job " << lastJob << std::endl;
  _jobQueue.push( lastJob  );
}

bool
JobManager::finishFrame() {
  
  ++_currentFrame;
  double nsec = _frameTimer.time();
  int framesLeft = Options::Instance()->numFrames() - _currentFrame;
  double eta = 0;
  if ( framesLeft > 0 )
    eta = nsec * framesLeft;
  
  std::cerr << "Frame " << _currentFrame << " of " << Options::Instance()->numFrames() << " rendered in "
	    << nsec << " seconds. ETA is " << eta << " sec." << std::endl;
    
  for ( FrameFuncs::iterator it = _funcs.begin();
	it != _funcs.end();
	++it )
    (**it)( _currentFrame );

  _frameTimer.restart();
  return (framesLeft > 0);
}

bool
JobManager::getNextRenderingJob( Job &job ) {

  // New way.
  _queueMutex->lock();
  
  // If the queue is empty, mark tracer as done.
  if ( _jobQueue.empty() ) {
    _tracersDone--;

    // If all the tracers are done, finish the frame.
    if ( _tracersDone == 0 ) {
      bool moreRendering = finishFrame();

      // If more rendering to be done, rebuild the job queue
      if ( moreRendering ) {
	createJobQueue();
      }
      _queueMutex->unlock();
      
      // If no more rendering, set a flag such that all further job
      // requests will fail.
      _noMoreJobs = !moreRendering;

      // And start everyone up again.
      _frameWaiter->signal();

      // And let everyone else know that we're out of jobs (done!)
      _jobQueueWaiter->signal();
    }
    // Otherwise, wait until everyone else is done
    else {
      _queueMutex->unlock();
      _frameWaiter->wait();
    }

    // If there are no more jobs, then just punt.
    if ( _noMoreJobs )
      return false;

    // Else, try this again...
    return getNextRenderingJob( job );
  }
  
  // Copy the job
  job = _jobQueue.front();
  _jobQueue.pop();
  
  if ( Options::Instance()->quiet() && Options::Instance()->isSlave() )
    std::cerr << "Getting job " << job.startx << ", " << job.starty << " -> "
	      << job.endx << ", " << job.endy << std::endl;
  _queueMutex->unlock();
  
  // Job copied successfully.
  return true;
}
