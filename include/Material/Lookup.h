#ifndef Lookup_h_
#define Lookup_h_

class Color;
class RenderContext;
class Ray;
class HitRecord;

class Lookup {
public:
  virtual void operator()(Color& result,
			  RenderContext& context,
			  const Ray& ray,
			  const HitRecord& hit,
			  const Color& atten,
			  int depth) const = 0;
  
  virtual ~Lookup() {}
protected:
  Lookup() {}
};

#endif
