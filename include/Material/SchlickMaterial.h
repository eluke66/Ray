
#ifndef SchlickMaterial_h
#define SchlickMaterial_h

#include "Material/Material.h"
#include "Material/SolidColor.h"
#include "Material/SchlickLighting.h"

typedef _Material<SolidColor,SchlickLighting> _SchlickMaterial;
#define SchlickMaterial(a,b,c,d)					\
  _SchlickMaterial(SolidColor(a),SchlickLighting(b,c,d))

#endif
