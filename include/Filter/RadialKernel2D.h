
#ifndef RadialKernel2D_h
#define RadialKernel2D_h

#include <math.h>

template<class Kernel>
class RadialKernel2D {
 public:
  RadialKernel2D() {}
  RadialKernel2D(const Kernel& filter) : filter(filter) {}
  ~RadialKernel2D() {}

  float getXSupport() const {
    return filter.getSupport();
  }
  float getYSupport() const {
    return filter.getSupport();
  }
  float getWeight(float x, float y) const {
    return filter.getWeight(sqrt(x*x+y*y));
  }
 private:
  Kernel filter;
};

#endif
