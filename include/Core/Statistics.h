#ifndef _STATISTICS_H_
#define _STATISTICS_H_

#include <sys/time.h>
#include "../config.h"
#include <iostream>

#define TRACE_ALLOCATIONS
#ifdef TRACE_ALLOCATIONS
#include <map>
#endif


#ifdef NO_STATISTICS
#define INC_STAT(a)
#define DEC_STAT(a)
#define SET_STAT(a,b)
#define ADD_STAT(a,b)
#define ALLOC
#define DALLOC(a)
#else
#define INC_STAT(a) Statistics::StatsObject.a++
#define DEC_STAT(a) Statistics::StatsObject.a--;
#define SET_STAT(a,b) (Statistics::StatsObject.a = (b))
#define ADD_STAT(a,b) (Statistics::StatsObject.a += (b))

#ifdef TRACE_ALLOCATIONS

#define ALLOC {Statistics::StatsObject.AllocCount[__func__]++;Statistics::StatsObject.SizeMap[__func__] += sizeof(*this);}
#define DALLOC(a) {Statistics::StatsObject.AllocCount[#a]--;Statistics::StatsObject.SizeMap[#a] -= sizeof(*this);}
#else
#define ALLOC
#define DALLOC(a)

#endif

#endif

class StatHolder {
  friend std::ostream& operator<<(std::ostream &out, const StatHolder &s);
  friend std::istream& operator>>(std::istream &in, StatHolder &s );
  
 public:
  StatHolder();
  ~StatHolder() {}
  StatHolder& operator+=(const StatHolder &s);
  
#ifdef HAVE_UNSIGNED_LONG_LONG
  typedef unsigned long long Stat;
#else
  typedef unsigned long Stat;
#endif
  
  typedef float TimeStat;

  
  // Rays
  Stat RaysTotal;
  Stat RaysEye;
  Stat RaysLight;
  Stat RaysReflected;
  Stat RaysTransmitted;

  // Intersections
  Stat BBoxIntersectionTests;
  Stat BBoxSuccessfulIntersections;
  Stat SphereIntersectionTests;
  Stat SphereSuccessfulIntersections;
  Stat PolygonIntersectionTests;
  Stat PolygonSuccessfulIntersections;
  Stat PlaneIntersectionTests;
  Stat PlaneSuccessfulIntersections;
  Stat ObjectIntersectionTests;
  Stat ObjectSuccessfulIntersections;

  // Caches
  Stat ShadowCacheHits;
  Stat ShadowCacheTests;
  
  // Other
  Stat ScreenPixels;
  Stat PixelsRendered;
  
  // Times
  TimeStat RenderTime;
  TimeStat LightingTime;
  TimeStat PreprocessTime;

  // Memory
  Stat StartOfDynamicMemory;
  Stat EndOfDynamicMemory;

  // System
  Stat NumericalErrors;

#ifdef TRACE_ALLOCATIONS
  typedef std::map<std::string, Stat> AllocMap;
  AllocMap AllocCount;
  AllocMap SizeMap;
#endif
  
  void report();
  void initialize();
  void startPreProcessing();
  void endPreProcessing();
  void endRendering();
  
 private:
  struct timeval StartOfPreProcessing;
  struct timeval EndOfPreProcessing;
  struct timeval EndOfRendering;
};

class Statistics {
public:
  static StatHolder StatsObject;
  
  
  static void Report() { StatsObject.report(); }
  static void Initialize() { StatsObject.initialize(); }
  static void StartPreProcessing() { StatsObject.startPreProcessing(); }
  static void EndPreProcessing() { StatsObject.endPreProcessing(); }
  static void EndRendering() { StatsObject.endRendering(); }
  static void AddStats( const StatHolder &stats ) { StatsObject += stats; }
  
  static StatHolder::TimeStat TimeDifference( struct timeval a, struct timeval b );
private:
  Statistics() {}
  ~Statistics() {}

 
};

#ifndef timersub
#define timersub(a, b, result)						      \
  do {									      \
    (result)->tv_sec = (a)->tv_sec - (b)->tv_sec;			      \
    (result)->tv_usec = (a)->tv_usec - (b)->tv_usec;			      \
    if ((result)->tv_usec < 0) {					      \
      --(result)->tv_sec;						      \
      (result)->tv_usec += 1000000;					      \
    }									      \
  } while (0)

#endif // TIMERSUB

#ifndef timeradd
# define timeradd(a, b, result)						      \
  do {									      \
    (result)->tv_sec = (a)->tv_sec + (b)->tv_sec;			      \
    (result)->tv_usec = (a)->tv_usec + (b)->tv_usec;			      \
    if ((result)->tv_usec >= 1000000)					      \
      {									      \
	++(result)->tv_sec;						      \
	(result)->tv_usec -= 1000000;					      \
      }									      \
  } while (0)
#endif // TIMERADD

#endif
