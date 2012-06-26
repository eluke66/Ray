from Core import *
from Background import *
from Materials import *
from Geometry import *
from IO import *
from Filter import *


scene = Scene()
world = Group();
scene.setBackground( ConstantBackground( Color (.745, 0.462, 0.313 ) ) )

mat = MultiGradientColor2D(11)
mat.addPoint( Point2D(0.5, 0.5), Color(1,0,0) )
mat.addPoint( Point2D(0.0, 0.0), Color(0.0) )
mat.addPoint( Point2D(1.0, 0.0), Color(0.0) )
mat.addPoint( Point2D(1.0, 1.0), Color(0.0) )
mat.addPoint( Point2D(0.0, 1.0), Color(0.0) )

b = Box( multiGradient2D(mat), Point(-17,-10,-10), Point(-7,10,10) )
world.addObject(b)

mat = MultiGradientColor2D()
mat.addPoint( Point2D(0.2,0.2), Color(1,0,0) )
mat.addPoint( Point2D(0.35,.35), Color(1,1,0) )
mat.addPoint( Point2D(0.5,.5), Color(1,0,1) )
mat.addPoint( Point2D(0.8,.8), Color(0,0,1) )
b = Box( multiGradient2D(mat), Point(-5,-10,-10), Point(5,10,10) )
world.addObject(b)

value = 1
for x in range( 8, 20, 4 ):
    for z in range( -20, 25, 8 ):
        mat = MultiGradientColor2D(value)
        mat.addPoint( Point2D(0.2,0.2), Color(1,0,0) )
        mat.addPoint( Point2D(0.35,.35), Color(1,1,0) )
        mat.addPoint( Point2D(0.5,.5), Color(1,0,1) )
        mat.addPoint( Point2D(0.8,.8), Color(0,0,1) )
        b = Sphere( multiGradient2D(mat),
                         Point(x,-10,z), 1 ) 
        world.addObject(b)
        value += 0.75
        
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
