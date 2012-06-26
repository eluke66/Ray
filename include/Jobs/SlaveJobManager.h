#ifndef _SLAVE_JOB_MANAGER_H_
#define _SLAVE_JOB_MANAGER_H_

#include "Jobs/JobManager.h"

class SlaveJobManager : public JobManager {
 public:
  SlaveJobManager();
  ~SlaveJobManager();

  virtual void start();
  virtual bool getNextRenderingJob( Job &job );
};

#endif
