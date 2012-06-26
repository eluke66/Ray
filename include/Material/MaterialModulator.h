#ifndef MaterialModulator_h_
#define MaterialModulator_h_

#include <vector>
#include "Material/Material.h"

class Modulator;

typedef std::vector<Material *> MaterialList;

class MaterialModulator : public Material {
public:
  MaterialModulator( Modulator *mod, const MaterialList &materials ) :
    _modulator(mod), _materials(materials) {}

  virtual void shade(Color& result,
		     RenderContext& context,
		     const Ray& ray,
		     const HitRecord& hit,
		     const Color& atten,
		     int depth) const;
  virtual BounceType bounceRay( Ray &newDirection,
				Color &newColor,
				bool &diffuse,
				const HitRecord &hit,
				const Ray &incoming, RenderContext &context ) const;
protected:
  Modulator *_modulator;
  MaterialList _materials;
  MaterialModulator();
};

#endif
