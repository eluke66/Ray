
#include "Core/Thread.h"
#include <errno.h>
#include <iostream>
#include <string.h>
Thread::Thread( std::string name ) : _name(name) {


}

Thread::~Thread() {


}

void *
startHelp( void * arg ) {
  Thread * thread = (Thread *)(arg);
  assert(thread);

  return thread->run();
}

void
Thread::join() {
  assert( !_detached );

  if ( pthread_join( _thread,
		     NULL ) ) {
    std::cerr << "Error joining thread " << _name
	      << " : " << strerror( errno ) << std::endl;
  }
}

void
Thread::start( bool detach ) {
  _detached = detach;
  
  // Set detached state so the thread doesn't need to return
  // back to report status bits
  pthread_attr_init( &_attr );

  if ( detach ) 
    pthread_attr_setdetachstate( &_attr, PTHREAD_CREATE_DETACHED );
  else
    pthread_attr_setdetachstate( &_attr, PTHREAD_CREATE_JOINABLE );
  
  if ( pthread_create( &_thread, &_attr, startHelp, this) ) {
     std::cerr << "Error creating thread " << _name
	       << " : " << strerror( errno ) << std::endl;
  }
  

}


Mutex::Mutex(std::string name ) : _name(name)  {
  pthread_mutex_init(&_posix_mutex,0);

}

Mutex::~Mutex() {
  pthread_mutex_destroy(&_posix_mutex);

}

void
Mutex::lock() {
  pthread_mutex_lock(&_posix_mutex);
}

bool
Mutex::trylock() {
  return pthread_mutex_trylock(&_posix_mutex) != EBUSY;
}

void
Mutex::unlock() {
  pthread_mutex_unlock(&_posix_mutex);
}



MutexLocker::MutexLocker( Mutex &mutex ) : _mutex(mutex) {
  _mutex.lock();
}


MutexLocker::~MutexLocker() {
  _mutex.unlock();
}


Condition::Condition(Mutex &mutex) : _mutex(mutex) {
  pthread_cond_init(&_cond, NULL);
}

  
Condition::~Condition() {
  pthread_cond_destroy(&_cond);
}

  
void 
Condition::wait() {
  pthread_cond_wait(&_cond, &_mutex._posix_mutex);
}
  
int 
Condition::timedwait(unsigned long secs, unsigned long nanosecs) {
  timespec rqts = { secs, nanosecs };
  int rc = 0;
  
 again:
  pthread_cond_timedwait(&_cond,
			 &_mutex._posix_mutex, &rqts);
  if (rc == 0)
    return 1;

  if (rc == EINTR)
    goto again;

  if (rc == ETIMEDOUT)
    return 0;

  return 0; // For completeness, should never get here...
}

void 
Condition::signal() {
  pthread_cond_signal(&_cond);
}

  
void 
Condition::broadcast() {
  pthread_cond_broadcast(&_cond);
}
