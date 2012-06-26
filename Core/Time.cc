
#include "Core/Time.h"
#include <sys/time.h>
#include <errno.h>
#ifdef __linux
#include <time.h>
#endif
#include <stdexcept>
#include <string.h>
static struct timeval start_time;
static bool initialized = false;

void
Time::initialize()
{
  initialized=true;
  if(gettimeofday(&start_time, 0) != 0)
    throw std::runtime_error(std::string("gettimeofday failed: ")
                             +strerror(errno));
}

double Time::currentSeconds()
{
  if(!initialized)
    initialized = true;
  struct timeval now_time;
  if(gettimeofday(&now_time, 0) != 0)
    throw std::runtime_error(std::string("gettimeofday failed: ")
                             +strerror(errno));
  return (now_time.tv_sec-start_time.tv_sec)+(now_time.tv_usec-start_time.tv_usec)*1.e-6;
}
