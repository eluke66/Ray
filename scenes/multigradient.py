from Core import *
from Background import *
from Materials import *
from Geometry import *
from IO import *
from Filter import *


scene = Scene()
world = Group();
scene.setBackground( ConstantBackground( Color (.745, 0.462, 0.313 ) ) )

mat = MultiGradientColor()
mat.addPoint( 0.5, Color(1,0,0) )
mat.addPoint( 0.8, Color(0,0,1) )
b = Box( multiGradient(mat), Point(-17,-10,-10), Point(-7,10,10) )
world.addObject(b)

mat = MultiGradientColor()
mat.addPoint( 0.2, Color(1,0,0) )
mat.addPoint( 0.35, Color(1,1,0) )
mat.addPoint( 0.5, Color(1,0,1) )
mat.addPoint( 0.8, Color(0,0,1) )
b = Box( multiGradient(mat), Point(-5,-10,-10), Point(5,10,10) )
world.addObject(b)

mat = MultiGradientColor(False)
mat.addPoint( 0.1, Color(1,0,0) )
mat.addPoint( 0.2, Color(0,1,0) )
mat.addPoint( 0.3, Color(0,0,1) )
mat.addPoint( 0.4, Color(1,1,0) )
mat.addPoint( 0.5, Color(1,0,1) )
mat.addPoint( 0.6, Color(1,0.3,0.6) )
mat.addPoint( 0.7, Color(0,0,0) )
mat.addPoint( 0.8, Color(1,1,1) )
b = Box( multiGradient(mat), Point(7,-10,-10), Point(17,10,10) )
world.addObject(b)

scene.setObject( world );

scene.addLight(PointLight(Point(20, -30, 30), Color(.9)));
scene.addLight(PointLight(Point(0, 0, 10), Color(.9)));

scene.setAmbient(Color(0.4))
scene.setCamera(PinholeCamera(Point(0, -60, 12.5),
                              Point(0, 0, 0),
                              Vector(0, 0, 1),
                              50));
scene.setMaxRayDepth(10);
scene.setMinAttenuation(.01);
