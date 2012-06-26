from Core import *
from Background import *
from Materials import *
from Geometry import *
from IO import *
from Filter import *

def bg3():
    mg = MultiGradientColor(False)
    pt = 0.45
    mg.addPoint( pt, Color(1) )
    pt += 0.05 ; mg.addPoint( pt, Color(1,0.8,0.8) )
    pt += 0.005 ; mg.addPoint( pt, Color(1,0.7,0.3) )
    pt += 0.015 ; mg.addPoint(pt, Color(0.8,0.5,0.6) )
    pt += 0.02 ; mg.addPoint( pt, Color(0.4,0.2,0.8) )
    pt += 0.025 ; mg.addPoint( pt, Color(0.2,0.1,0.8) )
    pt += 0.025 ; mg.addPoint( pt, Color(0.1,0.05,0.8) )
    mg.addPoint( 1.0, Color(0) )
    return MultiGradientBackground( mg )
    
def bg2():
    return GradientBackground( Color(0), Color(0,0,1), False )

def bg1():
    return ConstantBackground( Color (.745, 0.462, 0.313 ) )

scene = Scene()
world = Group();
scene.setBackground( bg3() )

world.addObject(Plane(lambertianMaterial(Color(0.5),0.6, 0.4), Vector(0,0,1), Point(0,0,0) ) )
scene.setObject( world );

scene.addLight(PointLight(Point(20, -30, 30), Color(.9)));
scene.addLight(PointLight(Point(0, 0, 10), Color(.9)));

scene.setAmbient(Color(0.4))
scene.setCamera(PinholeCamera(Point(0, -60, 6),
                              Point(0, 0, 0),
                              Vector(0, 0, 1),
                              50));
scene.setMaxRayDepth(10);
scene.setMinAttenuation(.01);
