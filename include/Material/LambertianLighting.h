#ifndef LambertianLighting_h
#define LambertianLighting_h

class HitRecord;
class Ray;
class RenderContext;
class Color;

class LambertianLighting {
public:
  LambertianLighting(float Kd, float Ka) : _Kd(Kd), _Ka(Ka) {}
  void operator()(Color& result, RenderContext& context, const Ray& ray,
		  const HitRecord& hit, const Color& atten, int depth) const;
  bool bounce( Ray& newDirection, bool& diffuse, double random, const HitRecord &hit, const Ray &incoming) const;

private:
  float _Kd;
  float _Ka;
};


#endif
