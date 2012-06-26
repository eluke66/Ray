#ifndef _UTILS_H_
#define _UTILS_H_

#include <values.h>
#include <math.h>
#include <string>
#include <sstream>
#include <iostream>
#include <assert.h>
#include <stdlib.h>
#include "Core/Options.h"

#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define ABSMIN(a,b) (fabs(a) < fabs(b) ? (a) : (b))
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define ABSCLOSE(a,b,eps) (fabs( (a) - (b) ) <= (eps) )  
#define IN_RANGE( a, mx, mn, eps ) \
  ( (a) <= ((mx)+(eps)) && (a) >= ((mn)-(eps)) )

#define NOT_DONE(s) (std::cerr << "NOT DONE: " << s << " at " << __FILE__ <<  " , line " << __LINE__ << std::endl)
#define ERROR(s) std::cerr << "Error: " << s << std::endl
#define WARN(s)  std::cerr << "Warning: " << s << std::endl

#ifdef  DO_DEBUG
#define DEBUG(s) std::cerr << "Debug: " << s << std::endl;
#else
#define DEBUG(s) 
#endif // DO_DEBUG


#define PRINT_TIMES(s, numTimes) \
 {\
  assert( numTimes > 0 ); \
  static unsigned aa ## __LINE__ = numTimes; \
  if ( aa ## __LINE__ > 0 ) {\
    if ( !Options::Instance()->quiet() )	\
     std::cerr << s << std::endl; \
   aa ## __LINE__--;\
  }\
 }

#define PRINT_ONCE(s) PRINT_TIMES(s, 1)

#define MOUSE_DEBUG
#ifdef MOUSE_DEBUG

#define mdebug(a) { if ( getenv("MDEBUG") ) { std::cerr << a << std::endl; } }
#define mdebugOn  setenv("MDEBUG","",1)
#define mdebugOff unsetenv("MDEBUG")

#else  // MOUSE_DEBUG

#define mdebug(a)
#define mdebugOn
#define mdebugOff

#endif // MOUSE_DEBUG



#define DEGTORAD(d) ( d / 180.0 * M_PI )
#define RADTODEG(r) ( r * 180.0 / M_PI )
#define BUFFER_SIZE 1024

#define READLN(input,infile) \
 do { \
    infile >> input; \
    if ( input[0] == '#' ) {\
      infile.getline( g_buffer, BUFFER_SIZE );\
      continue;\
    }\
    toLower(input);\
    break;\
 } while (1);\

#define READLINE(in,buffer) \
  do { \
    fscanf(in, "%s", buffer); \
    if ( buffer[0] == '#' ) { \
      fgets( buffer, BUFFER_SIZE, in );\
    }\
    else break; \
  } while (1);

#define FLUSHLINE(in) \
    in.getline( g_buffer, BUFFER_SIZE ); 

inline double clamp(double d) {
    if (d > 1.0) return 1.0;
    if (d < 0.0) return 0.0;
    return d;
}

template <typename T>
T ClampRange(T min, T max, T value ) {
  if ( value < min )
    return min;
  if ( value > max )
    return max;
  return value;
}

inline float clampf(float d) {
    if (d > 1.0f) return 1.0f;
    if (d < 0.0f) return 0.0f;
    return d;
}


inline void
toLower( std::string &str ) {
  for ( unsigned i = 0; i < str.length(); i++ )
    str[i] = tolower( str[i] );

}

inline double randRange( double min=0.0, double max=1.0 ) {
  double d = drand48();
  d *= max-min;
  d -= min;
  if ( d > max ) d = max;
  if ( d < min ) d = min;
  return d;
}

inline float GaussianRandom(float s=1.0, float m=0.0)	
{				        /* mean m, standard deviation s */
	float x1, x2, w, y1;
	static float y2;
	static int use_last = 0;

	if (use_last)		        /* use value from previous call */
	{
		y1 = y2;
		use_last = 0;
	}
	else
	{
		do {
			x1 = 2.0 * drand48() - 1.0;
			x2 = 2.0 * drand48() - 1.0;
			w = x1 * x1 + x2 * x2;
		} while ( w >= 1.0 );

		w = sqrt( (-2.0 * log( w ) ) / w );
		y1 = x1 * w;
		y2 = x2 * w;
		use_last = 1;
	}

	return( m + y1 * s );
}

// Generates a random value centered around 'mean', that is clamped to
// between 'mean' and 'bound'. 
inline float GaussianClamp( float bound, float stddev, float mean ) {
  float a = GaussianRandom( stddev, mean );

  if ( bound < mean ) {
    if ( a < bound )
      a = bound;
    if ( a > mean )
      a = mean;
  }
  else {
    if ( a > bound )
      a = bound;
    if ( a < mean )
      a = mean;
  }
  
  return a;
}

inline int sign( double d ) {
  if ( d >= 0.0f )
    return 1;
  return -1;
}
// Constants

// Default shading values
const float KA = 0.15;  // Ambient
const float KD = 0.5;   // Diffuse
const float KS = 0.35;  // Specular
const float KT = 0.8;
const float EXP = 15;   // Specular exponent

struct ShadingTerms {
  float _ka;
  float _kd;
  float _ks;
  float _kt;
  float _exp;
  float _atmos1;
  float _atmos2;
};


// Refraction values
const double N_AIR = 1.0;
const double N_GLASS = 1.6;

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

inline bool verifyFile( std::string filename ) {
  struct stat buf;
  
  int returnVal = stat( filename.c_str(), &buf );
  return (returnVal != -1 && S_ISREG( buf.st_mode ) );
}
#endif 
