
#ifndef OrenNayarMaterial_h
#define OrenNayarMaterial_h

#include "Material/Material.h"
#include "Core/Color.h"
#include "Material/SolidColor.h"
#include "Material/OrenNayarLighting.h"

typedef _Material<SolidColor,OrenNayarLighting> _OrenNayarMaterial;
#define OrenNayarMaterial(a,b) \
  _OrenNayarMaterial(a,b)


#endif
