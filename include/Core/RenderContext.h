
#ifndef RenderContext_h
#define RenderContext_h

#include "Core/Color.h"
class Scene;

class RenderContext {
 public:
  RenderContext(const Scene* scene)
    : scene(scene)
  {
  }
  ~RenderContext() {
  }

  const Scene* getScene() const {
    return scene;
  }
  
  Color originalColor;
  Color pigment;
  
 private:
  const Scene* scene;
};

#endif
