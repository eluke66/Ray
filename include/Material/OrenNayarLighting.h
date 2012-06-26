#ifndef OrenNayarLighting_h
#define OrenNayarLighting_h

class HitRecord;
class Ray;
class RenderContext;
class Color;

class OrenNayarLighting {
public:
  OrenNayarLighting(float sigma, float Kd=0.6, float Ka=0.4);
  void operator()(Color& result, RenderContext& context, const Ray& ray,
		  const HitRecord& hit, const Color& atten, int depth) const;
  bool bounce( Ray& newDirection, bool& diffuse, double random, const HitRecord &hit, const Ray &incoming) const;

private:
  double _A, _B;
  float _Kd;
  float _Ka;
};


#endif
