
#ifndef Filter1DWrapper_h
#define Filter1DWrapper_h

#include "Filter/Filter1D.h"

template<class Kernel> class Filter1DWrapper : public Filter1D {
 public:
  Filter1DWrapper() {}
  Filter1DWrapper(const Kernel& kernel) : kernel(kernel) {}
  virtual ~Filter1DWrapper() {}


  virtual float getSupport() const {
    return kernel.getSupport();
  }
  virtual float getWeight(float x) const {
    return kernel.getWeight(x);
  }
  virtual void getWeights(std::vector<float>& weights,
                          const std::vector<float>& xs) const {
    int n = weights.size();
    for(int i=0;i<n;i++)
      weights[i] = kernel.getWeight(xs[i]);
  }
 private:
  Filter1DWrapper(const Filter1DWrapper&);
  Filter1DWrapper& operator=(const Filter1DWrapper&);

  Kernel kernel;
};

#endif
