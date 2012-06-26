
#ifndef PhongVolumeMaterial_h
#define PhongVolumeMaterial_h

#include "Core/Array3.h"
#include "Core/Color.h"
#include "Core/Colormap.h"
#include "Core/Point.h"
#include "Core/Vector.h"
#include "Material/VolumeMaterial.h"
#include "Material/PhongLighting.h"


typedef _Material< VolumeMaterial<PhongShader> > _PhongVolumeMaterial;

// H and G are intentionally switched...
#define PhongVolumeMaterial(a,b,c,d,e,f,g,h,i,j)			\
  _PhongVolumeMaterial( VolumeMaterial<PhongShader>(a, b, c, d, e, f, h, PhongShader(g,i,j)))

#endif
