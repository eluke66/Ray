#ifndef BasicShade_h
#define BasicShade_h

#include "Material/Material.h"
#include "Material/SolidColor.h"

typedef _Material<SolidColor> _BasicShade;
#define BasicShade(a) _BasicShade(SolidColor(a))

#endif
