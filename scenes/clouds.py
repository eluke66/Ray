from Core import *
from Background import *
from Materials import *
from Geometry import *
from IO import *
from Filter import *

scene = Scene()
world = Group();

if ( False ):
    scene.setBackground( ConstantCloudBackground( SolidColor(Color(0,0,0.8)),
                                                  SolidColor(Color(1)),
                                                  0.1 ) )
else:
    gc = GradientColor(Color(0.3,0,0.8), Color(0,0,0.5), False )
    scene.setBackground( GradientCloudBackground( gc,
                                                  SolidColor(Color(1)),
                                                  0.1 ) )

scene.setObject( world );

scene.addLight(PointLight(Point(0, 0, 10), Color(.9)));

scene.setAmbient(Color(0.4))
scene.setCamera(PinholeCamera(Point(0, -60, 12.5),
                              Point(0, 0, 0),
                              Vector(0, 0, 1),
                              50));
scene.setMaxRayDepth(10);
scene.setMinAttenuation(.01);
