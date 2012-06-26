
#ifndef CheckerMaterial_h
#define CheckerMaterial_h

#include "Material/Material.h"
#include "Material/CheckerColor.h"

typedef _Material<CheckerColor> _CheckerMaterial;
#define CheckerMaterial(a,b,c,d,e) \
  _CheckerMaterial(CheckerColor(a,b,c,d,e))

#endif
