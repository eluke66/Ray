
#ifndef TextureBackground_h
#define TextureBackground_h

#include "Background/UVWBackground.h"
#include "Core/Color.h"
#include "Material/TextureColor.h"


class TextureBackground : public UVWBackground {
 public:
  TextureBackground(const std::string& textureName);
  virtual ~TextureBackground();

  virtual void preprocess();
  virtual void getBackgroundColor(Color& result, const RenderContext& context, const Ray& ray);

private:
  TextureBackground(const TextureBackground&);
  TextureBackground& operator=(const TextureBackground&);

  TextureColor _mat;
};

#endif

