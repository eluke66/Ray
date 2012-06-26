
#ifndef BoxWindow1D_h
#define BoxWindow1D_h

#include <math.h>
#include "Core/Math.h"

template<class Filter>
class BoxWindow1D {
 public:
  BoxWindow1D(Filter filter, float width) 
    : filter(filter) {
    halfwidth = width/2;
    offset = filter.getWeight(halfwidth);
    scale = 1./(1-offset);
  }
  BoxWindow1D(float width) {
    halfwidth = width/2;
    offset = filter.getWeight(halfwidth);
    scale = 1./(1-offset);
  }

  ~BoxWindow1D() {}

  float getSupport() const { return halfwidth*2; }
  float getWeight(float x) const {
    if(Abs(x) > halfwidth)
      return 0;
    else
      return filter.getWeight(x)*scale-offset;
  }
 public:
  Filter filter;
  float halfwidth;
  float scale;
  float offset;
};

#endif
