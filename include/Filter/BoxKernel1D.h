
#ifndef BoxKernel1D_h
#define BoxKernel1D_h

#include "Core/Math.h"

class BoxKernel1D {
 public:
  BoxKernel1D() {}
  ~BoxKernel1D() {}

  float getSupport() const { return 1; }
  float getWeight(float x) const {
    x = Abs(x);
    return x >= 0.5f ? 0.0f : 1.0f;
  }
 private:
};

#endif
