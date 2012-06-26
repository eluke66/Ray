
#ifndef TriangleKernel1D_h
#define TriangleKernel1D_h

#include "Core/Math.h"

class TriangleKernel1D {
 public:
  TriangleKernel1D() {}
  ~TriangleKernel1D() {}

  float getSupport() const { return 2.0f; }
  float getWeight(float x) const {
    x = Abs(x);
    return  x >= 1.0f? 0.0f : 1.0f-x;
  }
 private:
};

#endif
