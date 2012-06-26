
#ifndef PhongWoodMaterial_h
#define PhongWoodMaterial_h

#include "Material/Material.h"
#include "Material/WoodColor.h"
#include "Material/PhongLighting.h"

typedef _Material<WoodColor,PhongLighting> _PhongWoodMaterial;
#define PhongWoodMaterial(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o) \
  _PhongWoodMaterial(WoodColor(a,b,c,d,e,f,g,h,i,j,k),PhongLighting(l,m,n,o))

#endif
