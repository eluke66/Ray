#ifndef _GlobalIllumMat_h_
#define _GlobalIllumMat_h_

class Color;
class RenderContext;
class Ray;
class HitRecord;

class GlobalIllumMat {
public:
  void operator()(Color& result,RenderContext& context,
		  const Ray& ray, const HitRecord& hit,
		  const Color& atten, int depth) const;
  
};

#endif
