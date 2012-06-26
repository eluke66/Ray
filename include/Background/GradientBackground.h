
#ifndef GradientBackground_h
#define GradientBackground_h

#include "Background/UVWBackground.h"
#include "Core/Color.h"
#include "Material/GradientColor.h"

class GradientBackground : public UVWBackground {
 public:
  // Repeat this number of times.
  GradientBackground(const Color& tr, const Color &tl,
		     bool horizontal=true, float repeat=1.0 );
  GradientBackground( const Color& tr, const Color &tl,
		      const Color& br, const Color &bl,
		      float hRepeat=1.0, float vRepeat=1.0);
  virtual ~GradientBackground();

  virtual void preprocess();
  virtual void getBackgroundColor(Color& result, const RenderContext& context, const Ray& ray);

private:
  GradientBackground(const GradientBackground&);
  GradientBackground& operator=(const GradientBackground&);

  GradientColor _mat;
};

class MultiGradientBackground : public UVWBackground {
public:
  MultiGradientBackground(const MultiGradientColor &mg );

  virtual void preprocess();
  virtual void getBackgroundColor(Color& result, const RenderContext& context, const Ray& ray);

private:
  MultiGradientBackground(const MultiGradientBackground&);
  MultiGradientBackground& operator=(const MultiGradientBackground&);

  MultiGradientColor _mat;
};


#endif

