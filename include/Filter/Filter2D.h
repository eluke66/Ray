
#ifndef Filter2D_h
#define Filter2D_h

#include <vector>

class Filter2D {
 public:
  Filter2D();
  virtual ~Filter2D();
  
  virtual float getXSupport() const = 0;
  virtual float getYSupport() const = 0;
  virtual float getWeight(float x, float y) const = 0;
  virtual void getWeights(std::vector<float>& weights,
                          const std::vector<float>& xs,
                          const std::vector<float>& ys) const = 0;
 private:
  Filter2D(const Filter2D&);
  Filter2D& operator=(const Filter2D&);
};


#endif
