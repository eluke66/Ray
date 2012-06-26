#ifndef RefractingDielectric_h
#define RefractingDielectric_h

class Color;
class RenderContext;
class Ray;
class HitRecord;
class Blur;
class Vector;

class RefractingDielectric {
public:
  RefractingDielectric(double eta, Blur *blur=NULL);
  
  void operator()(Color& result, RenderContext &context, const Ray& ray,
		  const HitRecord& hit, const Color& atten, int depth) const;
  bool bounce( Ray& newDirection, bool& diffuse, double random,
	       const HitRecord &hit, const Ray &incoming) const;
  bool generatesCaustics() const { return true; }
  void  getBlur( RenderContext &context,
		 const Ray &ray,
		 const HitRecord &hit,
		 double &blur, int &numSamples ) const;
  Color computeColor( const Vector &normal, RenderContext &context,
		       const Ray& ray,
		       const HitRecord& hit,
		       const Color& atten, int depth) const;
private:
  double _eta;
  double _inv_eta;
  double _r0;
  Blur  *_blur;
};



#endif
