
#ifndef StarfieldBackground_h
#define StarfieldBackground_h

#include "Background/Background.h"
#include "Core/Color.h"

class StarfieldBackground : public Background {
 public:
  StarfieldBackground(const Color& color);
  virtual ~StarfieldBackground();

  virtual void preprocess();
  virtual void getBackgroundColor(Color& result, const RenderContext& context, const Ray& ray);

 private:
  StarfieldBackground(const StarfieldBackground&);
  StarfieldBackground& operator=(const StarfieldBackground&);

  Color bgcolor;
};

#endif

