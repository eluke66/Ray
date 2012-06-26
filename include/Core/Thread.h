#ifndef _THREAD_H_
#define _THREAD_H_

#include <pthread.h>
#include <assert.h>
#include <string>
#include <queue>

///////////////////////////////////////////////////////////
//
// Thread class. Subclassed by things that want to be run in a
// separate thread.
//
class Thread {
 public:
  Thread(std::string name);
  virtual ~Thread();

  virtual void *run() = 0;

  // Waits until this thread is done. Will assert if the thread was
  // started undetached.
  void join();

  
  void start( bool detach );
 protected:

  bool           _detached;
  std::string    _name;
  pthread_t      _thread;
  pthread_attr_t _attr;

};

///////////////////////////////////////////////////////////
//
// Locking mutex.
//
class Mutex {
  friend class Condition;
 public:
  Mutex( std::string name  );
  ~Mutex();

  // Lock the mutex
  void lock();
  
  // Returns true if lock succeeds.
  bool trylock();

  
  void unlock();
 private:
  std::string _name;
  pthread_mutex_t _posix_mutex;
};

///////////////////////////////////////////////////////////
//
// Automatically locks mutex when created, unlocks when
// destroyed. Used for preventing deadlocks.
//
class MutexLocker {
public:
  MutexLocker( Mutex &mutex );
  ~MutexLocker();

private:
  Mutex &_mutex;
};


///////////////////////////////////////////////////////////
//
// Condition variable
//
class Condition {
public:
  Condition(Mutex &mutex);
  ~Condition();

  
  // wait for the condition variable to be signalled.  The mutex is
  // implicitly released before waiting and locked again after waking up.
  // If wait() is called by multiple threads, a signal may wake up more
  // than one thread.  See POSIX threads documentation for details.
  void wait();

  
  // timedwait() is given an absolute time to wait until.  To wait for a
  // relative time from now, use omni_thread::get_time. See POSIX threads
  // documentation for why absolute times are better than relative.
  // Returns 1 (true) if successfully signalled, 0 (false) if time
  // expired.
  int timedwait(unsigned long secs, unsigned long nanosecs = 0);

  
  // if one or more threads have called wait(), signal wakes up at least
  // one of them, possibly more.  See POSIX threads documentation for
  // details.
  void signal();

  
  // broadcast is like signal but wakes all threads which have called
  // wait().
  void broadcast();
private:
  Mutex &_mutex;
  pthread_cond_t _cond;
};

///////////////////////////////////////////////////////////
//
// Thread safe queue - WARNING - DOESN:T WORK! NEED A MUTEX OUTSIDE OF
// QUEUE TO BE THREAD SAFE!
//
template <typename T>
class SafeQueue {
public:
  SafeQueue( std::string name="" ) :_name(name) {
    _myMutex = new Mutex( name + "QMutex" );
  }
  ~SafeQueue() {
    delete _myMutex;
  }

  void push( const T &t ) {
    MutexLocker( *_myMutex );
    _queue.push(t);
  }
  
  T&   pop() {
    MutexLocker( *_myMutex );
    T& front = _queue.front();
    _queue.pop();
    return front;
  }

  int  length() {
    MutexLocker( *_myMutex );
    return _queue.size();
  }
  bool empty() {
    MutexLocker( *_myMutex );
    return _queue.empty();
  }
  
private:
  std::string _name;
  std::queue<T> _queue;
  Mutex *_myMutex;
};
#endif
