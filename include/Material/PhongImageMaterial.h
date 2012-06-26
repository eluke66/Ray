
#ifndef PhongImageMaterial_h
#define PhongImageMaterial_h

#include "Material/Material.h"
#include "Material/TextureColor.h"
#include "Material/PhongLighting.h"

typedef _Material<TextureColor,PhongLighting> _PhongImageMaterial;

#define PhongImageMaterial(a,b,c,d,e)				\
  _PhongImageMaterial(TextureColor(a),PhongLighting(b,c,d,e))

#define RepeatingPhongImageMaterial( a, a1, a2, b, c, d, e ) \
  _PhongImageMaterial(TextureColor(a, a1, a2),PhongLighting(b,c,d,e))

#endif
