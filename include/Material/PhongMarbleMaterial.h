
#ifndef PhongMarbleMaterial_h
#define PhongMarbleMaterial_h

#include "Material/Material.h"
#include "Material/MarbleColor.h"
#include "Material/PhongLighting.h"


typedef _Material<MarbleColor,PhongLighting> _PhongMarbleMaterial;
#define PhongMarbleMaterial(a,b,c,d,e,f,g,h,i,j,k,l) \
  _PhongMarbleMaterial(MarbleColor(a,b,c,d,e,f,g,h),PhongLighting(i,j,k,l))

#endif
