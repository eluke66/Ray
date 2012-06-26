
#ifndef PhongMaterial_h
#define PhongMaterial_h

#include "Material/Material.h"
#include "Material/PhongLighting.h"
#include "Material/SolidColor.h"

typedef _Material<SolidColor,PhongLighting> _PhongMaterial;
#define PhongMaterial(a,b,c,d,e) \
  _PhongMaterial(SolidColor(a), PhongLighting(b,c,d,e))

#endif
