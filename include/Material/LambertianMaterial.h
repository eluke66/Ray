
#ifndef LambertianMaterial_h
#define LambertianMaterial_h

#include "Material/Material.h"
#include "Material/SolidColor.h"
#include "Material/LambertianLighting.h"

typedef _Material<SolidColor,LambertianLighting,NoOp,GlobalIllumMat> _LambertianMaterial;
#define LambertianMaterial(a,b,c) \
  _LambertianMaterial(SolidColor(a),LambertianLighting(b,c),NoOp::None,GlobalIllumMat())

#endif
