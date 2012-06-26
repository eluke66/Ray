#include "Core/Statistics.h"
#include "Core/Options.h"
#include <iostream>
#include <unistd.h>
#include <sstream>
#include <fstream>

StatHolder
Statistics::StatsObject;

void formatBytes( StatHolder::Stat totalBytes, std::ostream &output ) {
  if ( totalBytes > 1000 )
    if ( totalBytes > 1000000 )
      if ( totalBytes > 1000000000 )
	output << (totalBytes / 1000000000) << " GB" << std::endl;
      else
	output << (totalBytes / 1000000) << " MB" << std::endl;
    else
      output << (totalBytes / 1000) << " kB" << std::endl;
  else
    output << totalBytes << " bytes." << std::endl;
}
			
StatHolder::StatHolder() : RaysTotal(0), RaysEye(0), RaysLight(0), RaysReflected(0),
			   RaysTransmitted(0), BBoxIntersectionTests(0),
			   BBoxSuccessfulIntersections(0), SphereIntersectionTests(0),
			   SphereSuccessfulIntersections(0),PolygonIntersectionTests(0),
			   PolygonSuccessfulIntersections(0),PlaneIntersectionTests(0),
			   PlaneSuccessfulIntersections(0),ObjectIntersectionTests(0),
			   ObjectSuccessfulIntersections(0), ShadowCacheHits(0),
			   ShadowCacheTests(0), ScreenPixels(0),PixelsRendered(0),
			   RenderTime(0), LightingTime(0), PreprocessTime(0),
			   StartOfDynamicMemory(0),EndOfDynamicMemory(0),
			   NumericalErrors(0) {

}

std::ostream& operator<<(std::ostream &out, const StatHolder &s) {
  out << s.RaysTotal << " " << s.RaysEye << " " << s.RaysLight << " "
      << s.RaysReflected << " " << s.RaysTransmitted << " ";
  out << s.BBoxIntersectionTests  << " " << s.BBoxSuccessfulIntersections
      << " " << s.SphereIntersectionTests << " " << s.SphereSuccessfulIntersections
      << " " << s.PolygonIntersectionTests << " " << s.PolygonSuccessfulIntersections
      << " " << s.PlaneIntersectionTests << " " << s.PlaneSuccessfulIntersections
      << " " << s.ObjectIntersectionTests << " " << s.ObjectSuccessfulIntersections << " ";
  out << s.ShadowCacheHits << " " << s.ShadowCacheTests << " ";
  out << s.ScreenPixels << " " << s.PixelsRendered << " ";
  out << s.RenderTime << " " << s.LightingTime << " "
      << s.PreprocessTime << " ";
  out << s.StartOfDynamicMemory << " " << s.EndOfDynamicMemory << " ";
  out << s.NumericalErrors << " ";

#ifdef TRACE_ALLOCATIONS
  out << s.AllocCount.size() << " ";
  for (StatHolder::AllocMap::const_iterator it = s.AllocCount.begin();
       it != s.AllocCount.end();
       ++it )
    {
      out << it->first << " " << it->second << " ";
    }
  for (StatHolder::AllocMap::const_iterator it = s.SizeMap.begin();
       it != s.SizeMap.end();
       ++it )
    {
      out << it->first << " " << it->second << " ";
    }
#endif
  
  return out;
}


std::istream& operator>>(std::istream &in, StatHolder &s ) {
  in >> s.RaysTotal >> s.RaysEye >> s.RaysLight
     >> s.RaysReflected >> s.RaysTransmitted;
  in >> s.BBoxIntersectionTests  >> s.BBoxSuccessfulIntersections
     >> s.SphereIntersectionTests >> s.SphereSuccessfulIntersections
     >> s.PolygonIntersectionTests >> s.PolygonSuccessfulIntersections
     >> s.PlaneIntersectionTests >> s.PlaneSuccessfulIntersections
     >> s.ObjectIntersectionTests >> s.ObjectSuccessfulIntersections;
  in >> s.ShadowCacheHits >> s.ShadowCacheTests;
  in >> s.ScreenPixels >> s.PixelsRendered;
  in >> s.RenderTime >> s.LightingTime
     >> s.PreprocessTime;
  in >> s.StartOfDynamicMemory >> s.EndOfDynamicMemory;
  in >> s.NumericalErrors;

#ifdef TRACE_ALLOCATIONS
  unsigned numItems;
  std::string name;
  StatHolder::Stat count;
  in >> numItems;
  for ( unsigned i = 0; i < numItems; ++i ) {
    in >> name >> count;
    s.SizeMap[name] = count;
  }
  for ( unsigned i = 0; i < numItems; ++i ) {
    in >> name >> count;
    s.AllocCount[name] = count;
  }
#endif
  
  return in;
}


StatHolder&
StatHolder::operator+=(const StatHolder &s) {
  RaysTotal += s.RaysTotal;
  RaysEye += s.RaysEye;
  RaysLight += s.RaysLight;
  RaysReflected += s.RaysReflected;
  RaysTransmitted += s.RaysTransmitted;

  // Intersections
  BBoxIntersectionTests += s.BBoxIntersectionTests ;
  BBoxSuccessfulIntersections += s.BBoxSuccessfulIntersections;
  SphereIntersectionTests += s.SphereIntersectionTests;
  SphereSuccessfulIntersections += s.SphereSuccessfulIntersections;
  PolygonIntersectionTests += s.PolygonIntersectionTests;
  PolygonSuccessfulIntersections += s.PolygonSuccessfulIntersections;
  PlaneIntersectionTests += s.PlaneIntersectionTests ;
  PlaneSuccessfulIntersections += s.PlaneSuccessfulIntersections;
  ObjectIntersectionTests += s.ObjectIntersectionTests;
  ObjectSuccessfulIntersections += s.ObjectSuccessfulIntersections;

  // Caches
  ShadowCacheHits += s.ShadowCacheHits;
  ShadowCacheTests += s.ShadowCacheTests;
  
  // Other
  ScreenPixels += s.ScreenPixels;
  PixelsRendered += s.PixelsRendered;
  
  // Times
  RenderTime += s.RenderTime;
  LightingTime += s.LightingTime;
  PreprocessTime += s.PreprocessTime;

  // Memory
  StartOfDynamicMemory += s.StartOfDynamicMemory;
  EndOfDynamicMemory += s.EndOfDynamicMemory ;

  // System
  NumericalErrors += s.NumericalErrors;

#ifdef TRACE_ALLOCATIONS
  for (AllocMap::const_iterator it = s.AllocCount.begin();
       it != s.AllocCount.end();
       ++it )
    {
      AllocCount[it->first] += it->second;
    }
  for (StatHolder::AllocMap::const_iterator it = s.SizeMap.begin();
       it != s.SizeMap.end();
       ++it )
    {
      SizeMap[it->first] += it->second;
    }
#endif
  
  return *this;
}

void
StatHolder::initialize() {
  StartOfDynamicMemory = (StatHolder::Stat)sbrk(0);
  startPreProcessing();
}

void
StatHolder::startPreProcessing() {
  gettimeofday( &StartOfPreProcessing, NULL );
}

void
StatHolder::endPreProcessing() {
  gettimeofday( &EndOfPreProcessing, NULL );

  struct timeval total;
  timersub( &EndOfPreProcessing, &StartOfPreProcessing, &total );
  PreprocessTime =
    total.tv_sec  * 1000.0 +
    total.tv_usec / 1000.0;
}

void
StatHolder::endRendering() {
  gettimeofday( &EndOfRendering, NULL );

  struct timeval total;
  timersub( &EndOfRendering, &EndOfPreProcessing, &total );
  RenderTime =
    total.tv_sec  * 1000.0 +
    total.tv_usec / 1000.0;
  EndOfDynamicMemory = (StatHolder::Stat)sbrk(0);
}

void
StatHolder::report() {

  RaysTotal = RaysEye + RaysLight + RaysReflected + RaysTransmitted;
  
  std::ostringstream output;
  
  output << "Rays: " << std::endl;
  output << "\tEye:         " << RaysEye << std::endl;
  output << "\tLight:       " << RaysLight << std::endl;
  output << "\tReflected:   " << RaysReflected << std::endl;
  output << "\tTransmitted: " << RaysTransmitted << std::endl;
  output << "\tTOTAL:       " << RaysTotal << std::endl;
  
  output << std::endl;

  output << "Caches: " << std::endl;
  output << "\tShadow: " << ShadowCacheHits << " / " << ShadowCacheTests
	 << " (" << 100.0 * (float)ShadowCacheHits / ShadowCacheTests << " %)" << std::endl;
  
  output << std::endl;
  
  output << "Intersections (successful/total/%): " << std::endl;
  output << "\tBounding Box: " << BBoxSuccessfulIntersections << " / " << BBoxIntersectionTests
       << " (" << 100.0 * (float)BBoxSuccessfulIntersections / BBoxIntersectionTests << " %)" << std::endl;
  output << "\tSphere:       " << SphereSuccessfulIntersections << " / " << SphereIntersectionTests
       << " (" << 100.0 * (float)SphereSuccessfulIntersections / SphereIntersectionTests << " %)" << std::endl;
  output << "\tPolygon:      " << PolygonSuccessfulIntersections << " / " << PolygonIntersectionTests
       << " (" << 100.0 * (float)PolygonSuccessfulIntersections / PolygonIntersectionTests << " %)" << std::endl;
  output << "\tPlane:        " << PlaneSuccessfulIntersections << " / " << PlaneIntersectionTests
       << " (" << 100.0 * (float)PlaneSuccessfulIntersections / PlaneIntersectionTests << " %)" << std::endl;
  output << "\tObject:       " << ObjectSuccessfulIntersections << " / " << ObjectIntersectionTests
       << " (" << 100.0 * (float)ObjectSuccessfulIntersections / ObjectIntersectionTests << " %)" << std::endl;
  
  output << std::endl;

  output << "Pixels: " << std::endl;
  output << "\tScreen Pixels: " << ScreenPixels << std::endl;
  output << "\tTotal Pixels Rendered : " << PixelsRendered << std::endl;
  output << "\tAverage Pixels Rendered per Screen Pixel: " << float(PixelsRendered) / ScreenPixels << std::endl;
  
  output << std::endl;

  float totalTime = RenderTime + PreprocessTime;
  output << "Times: " << std::endl;

  
  if ( PreprocessTime > 1000.0 )
    output << "\tPreprocessing: " << PreprocessTime / 1000.0 << " sec." << std::endl;
  else
    output << "\tPreprocessing: " << PreprocessTime << " ms" << std::endl;

  if ( RenderTime > 1000.0 )
    output << "\tRendering:     " << RenderTime / 1000.0 << " sec." << std::endl;
  else
    output << "\tRendering:     " << RenderTime  << " ms" << std::endl;

  // TEMP
  output << "\t\t(Lighting was " << LightingTime / 1000.0 << " sec)" << std::endl;

  if ( totalTime > 1000.0 )
    output << "\tTOTAL:         " << totalTime / 1000.0 << " sec." << std::endl;
  else
    output << "\tTOTAL:         " << totalTime << " ms" << std::endl;

  output << "Average Rays/Pixel : " << (float)RaysTotal / ScreenPixels << std::endl;
  output << "Average Time/Pixel : " << (float)RenderTime / ScreenPixels << " ms" << std::endl;

  output << "Total memory used : ";
  
  Stat totalBytes = EndOfDynamicMemory - StartOfDynamicMemory;
  formatBytes( totalBytes, output );
  output << "Total numerical errors : " << NumericalErrors << std::endl;

  output << "Random Seed: " << Options::Instance()->randomSeed() << std::endl;
#ifdef TRACE_ALLOCATIONS
  totalBytes = 0;
  for (AllocMap::const_iterator it = AllocCount.begin();
       it != AllocCount.end();
       ++it )
    {
      if ( it->second != 0 ) {
	output << "Allocated " << it->second << " " << it->first << " items requiring ";
	formatBytes( SizeMap[it->first], output );
	totalBytes += SizeMap[it->first];
      }
      
    }
  output << "Total allocations: ";
  formatBytes(totalBytes,output);
#endif
  
  std::string ostr = output.str();
  std::cerr << ostr;

  std::string of = Options::Instance()->outputFilename();
  std::ofstream outfile( std::string(of + ".statistics").c_str() );
  outfile << ostr;
  outfile.close();
}

StatHolder::TimeStat
Statistics::TimeDifference( struct timeval a, struct timeval b ) {
  struct timeval total;

  timersub( &b, &a, &total );
  
  return total.tv_sec * 1000.0 +
    total.tv_usec / 1000.0;
}
