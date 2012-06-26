#ifndef _REMOTE_JOB_MANAGER_H_
#define _REMOTE_JOB_MANAGER_H_

#include <vector>
#include <string>

#include "UI/UserInterface.h"
#include "Jobs/JobManager.h"
#include "Core/Options.h"

typedef std::vector<std::string> StringVector;
class Job;

class RemoteJobManager {
  friend class RemoteThread;
 public:
  RemoteJobManager( const Options::SlaveList &slaves,
		    UI & ui, JobManager &mgr ) :
    _slaves(slaves), _ui(ui), _mgr(mgr) {}
  
  void start();
  void abort();
  inline unsigned getNumRemotes() const { return _slaves.size(); }

  void waitForFinish();
 private:
  const Options::SlaveList &_slaves;
  UI &_ui;
  JobManager &_mgr;
  
  typedef std::vector<Thread *> ThreadList;
  ThreadList _threadList;
  
  inline bool getNextRenderingJob( Job &job ) { return _mgr.getNextRenderingJob(job); }
  inline void putPixel( const Color &c, int x, int y ) {
    _ui.putPixel(c,x,y);
  }
};

#endif
