#ifndef TextureColor_h
#define TextureColor_h

class Image;
class HitRecord;
class Ray;
class RenderContext;
class Color;
class Vector;

#include <string>

class TextureColor {
public:
  // Repeats = how often the texture will repeat. 
  TextureColor(const std::string& imagename, double uRepeat = 1.0, double vRepeat = 1.0f);
  TextureColor(const TextureColor& other, double uRepeat, double vRepeat);
  TextureColor(Image *image, double uRepeat = 1.0, double vRepeat = 1.0);
  void operator()(Color& result, RenderContext& context, const Ray& ray,
		  const HitRecord& hit, const Color& atten, int depth) const;
  
  void operator()(Color& result, const Vector &uvw ) const;
  
private:
  Image *_image;
  double _uRepeat, _vRepeat;
  TextureColor();
};

#endif
