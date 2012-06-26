
#ifndef SeparableKernel2D_h
#define SeparableKernel2D_h

template<class XKernel, class YKernel = XKernel>
class SeparableKernel2D {
 public:
  SeparableKernel2D() {}
  SeparableKernel2D(const XKernel& filter) : xfilter(filter), yfilter(filter) {}
  SeparableKernel2D(const XKernel& xfilter, const YKernel& yfilter)
 : xfilter(xfilter), yfilter(yfilter) {}
  ~SeparableKernel2D() {}

  float getXSupport() const {
    return xfilter.getSupport();
  }
  float getYSupport() const {
    return yfilter.getSupport();
  }
  float getWeight(float x, float y) const {
    return xfilter.getWeight(x) * yfilter.getWeight(y);
  }
 private:
  XKernel xfilter;
  YKernel yfilter;
};

#endif
