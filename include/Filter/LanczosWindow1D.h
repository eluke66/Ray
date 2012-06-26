
#ifndef LanczosWindow1D_h
#define LanczosWindow1D_h

#include <math.h>
#include "Core/Math.h"

template<class Filter>
class LanczosWindow1D {
 public:
  LanczosWindow1D(Filter filter, float numlobes) 
    : filter(filter), numlobes(numlobes) {
    wscale = M_PI/numlobes;
    hscale = numlobes/M_PI;
  }
  LanczosWindow1D(float numlobes) 
    : numlobes(numlobes) {
    wscale = M_PI/numlobes;
    hscale = numlobes/M_PI;
  }

  ~LanczosWindow1D() {}

  float getSupport() const { return numlobes*2; }
  float getWeight(float x) const {
    x = Abs(x);
    if(x < 1.e-6f)
      return filter.getWeight(x);
    else if(x >= numlobes)
      return 0;
    else
      return sin(x*wscale)*hscale/x*filter.getWeight(x);
  }
 public:
  Filter filter;
  float numlobes;
  float wscale;
  float hscale;
};

#endif
