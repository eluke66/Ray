
#ifndef Filter1D_h
#define Filter1D_h

#include <vector>

class Filter1D {
 public:
  Filter1D();
  virtual ~Filter1D();
  virtual float getSupport() const = 0;
  virtual float getWeight(float x) const = 0;
  virtual void getWeights(std::vector<float>& weights,
                          const std::vector<float>& xs) const = 0;
 private:
  Filter1D(const Filter1D&);
  Filter1D& operator=(const Filter1D&);
};

#endif
