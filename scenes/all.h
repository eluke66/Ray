#include "Background/Background.h"
#include "Geometry/Box.h"
#include "Core/Camera.h"
#include "Material/CheckerMaterial.h"
#include "Background/ConstantBackground.h"
#include "Core/Color.h"
#include "Material/DielectricMaterial.h"
#include "Geometry/Disk.h"
#include "Filter/Filters.h"
#include "Geometry/GriddedGroup.h"
#include "Geometry/Group.h"
#include "Geometry/Heightfield.h"
#include "Core/Image.h"
#include "Material/LambertianMaterial.h"
#include "Core/Light.h"
#include "Core/AreaLight.h"
#include "Core/DirectionalLight.h"
#include "Material/Material.h"
#include "Material/MetalMaterial.h"
#include "Geometry/Object.h"
#include "Material/OrenNayarMaterial.h"
#include "Material/PhongImageMaterial.h"
#include "Material/PhongMarbleMaterial.h"
#include "Material/PhongMaterial.h"
#include "Material/PhongVolumeMaterial.h"
#include "Material/PhongWoodMaterial.h"
#include "Core/PinholeCamera.h"
#include "Geometry/Plane.h"
#include "Core/PointLight.h"
#include "Geometry/QAEB.h"
#include "Core/Ray.h"
#include "Geometry/Ring.h"
#include "Core/Scene.h"
#include "Geometry/Sphere.h"
#include "Geometry/SpherePolar.h"
#include "Core/Time.h"
#include "Geometry/Triangle.h"
#include "IO/ImageLoader.h"
#include "Core/LightSphere.h"
#include "Core/ThinLensCamera.h"
#include "Material/BasicShade.h"
#include "IO/ObjectLoader.h"

#include <iostream>
extern void error();
extern SceneMakerMap *GetAllScenes();

#define ADD_SCENE(scene) \
  struct init##scene { \
    init##scene() { (*GetAllScenes())[ #scene ] = scene; }	\
  }; \
  static init##scene the##scene;




