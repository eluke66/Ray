
#ifndef GaussianKernel1D_h
#define GaussianKernel1D_h

#include <float.h>
#include <math.h>

class GaussianKernel1D {
 public:
  GaussianKernel1D() {}
  ~GaussianKernel1D() {}

  float getWeight(float x) const {
    return exp(-2*x*x);
  }
 private:
};

#endif
