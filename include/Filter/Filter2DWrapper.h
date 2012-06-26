
#ifndef Filter2DWrapper_h
#define Filter2DWrapper_h

#include "Filter/Filter2D.h"

template<class Kernel>
class Filter2DWrapper : public Filter2D {
 public:
  Filter2DWrapper() { }
  Filter2DWrapper(const Kernel& kernel) : kernel(kernel) {}
  virtual ~Filter2DWrapper() {}

  virtual float getXSupport() const {
    return kernel.getXSupport();
  }
  virtual float getYSupport() const {
    return kernel.getYSupport();
  }
  virtual float getWeight(float x, float y) const {
    return kernel.getWeight(x, y);
  }
  virtual void getWeights(std::vector<float>& weights,
                          const std::vector<float>& xs,
                          const std::vector<float>& ys) const {
    int n = weights.size();
    for(int i=0;i<n;i++)
      weights[i] = kernel.getWeight(xs[i], ys[i]);
  }
 private:
  Filter2DWrapper(const Filter2DWrapper&);
  Filter2DWrapper& operator=(const Filter2DWrapper&);

  Kernel kernel;
};

#endif
