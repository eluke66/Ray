#ifndef SchlickLighting_h
#define SchlickLighting_h

class HitRecord;
class Ray;
class RenderContext;
class Color;

class SchlickLighting {
  friend class SchlickShader;
public:
  // F0 = reflection at normal incidence, 0->1
  // Sigma = 0-1, roughness, with 0 = perfectly specular and 1 =
  // perfectly lambertian.
  // psi = 0-1, isotropy, 0 = anisotropic, 1 = isotropic.
  SchlickLighting(double F0, double sigma, double psi );
  void operator()(Color& result, RenderContext& context, const Ray& ray,
		  const HitRecord& hit, const Color& atten, int depth) const;
  bool bounce( Ray& newDirection, bool& diffuse, double random, const HitRecord &hit, const Ray &incoming) const;

private:
  double _F0, _sigma, _psi;
  double _g, _d, _s;
};


#endif
