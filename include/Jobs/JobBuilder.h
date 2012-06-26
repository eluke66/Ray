#ifndef JOB_BUILDER_H_
#define JOB_BUILDER_H_

#include "Job.h"

class Image;

class JobBuilder {
public:

  static void MakeJobsStripes(JobQueue &jobQueue, unsigned width, unsigned height, unsigned int numJobs, int startx=0, int starty=0);
  static void MakeJobsTiles(JobQueue &jobQueue, unsigned width, unsigned height, unsigned int numJobs, int startx=0, int starty=0);
  static void MakeJobsFromImage(JobQueue &jobQueue, Image *image, unsigned int numJobs );
    
private:
  JobBuilder();
  
};
#endif
