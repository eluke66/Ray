
#ifndef SincKernel1D_h
#define SincKernel1D_h

#include <math.h>
#include "Core/Math.h"

class SincKernel1D {
 public:
  SincKernel1D() {}
  ~SincKernel1D() {}

  float getWeight(float x) const {
    x = Abs(x);
    if(x < 1.e-6f)
      return 1;
    else
      return sin(x*(float)M_PI)/x/(float)M_PI;
  }
 private:
};

#endif
