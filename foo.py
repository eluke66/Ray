from Core import *
from Background import *
from Materials import *
from Geometry import *

scene = Scene()
scene.setBackground( ConstantBackground( Color(0.1, 0.2, 0.3) ) )
world = Group()
world.addObject( Sphere( lambertianMaterial(Color(1, .2, .2), .6, .4),
                         Point(-2, -0.5, 2), 1.2))
world.addObject( Sphere( lambertianMaterial(Color(.2, 1, 1), .6, .4),
                            Point(1.5, 1, 4), 2));
world.addObject( Sphere( lambertianMaterial(Color(1,0,1), .6, .4),
                            Point(25,25,120), 10));
world.addObject( Sphere( lambertianMaterial(Color(1,0,1), .6, .4),
                            Point(-5, -5, -20), 10));

mat1 = lambertianMaterial(Color(.8, .8, .8), .6, .4)
mat2 = lambertianMaterial(Color(1, 0, 0), 0.1, 0.4)
gm = MaterialModulator( GradientModulator(1.0,0.5), [mat2, mat1, mat1, mat2] )
world.addObject( Plane( gm,
                        Vector(0,0,1), Point(0,0,-.1)));
scene.setObject(world);

scene.setAmbient(Color(.4, .4, .4));
scene.addLight( PointLight(Point(20, 30, 100), Color(.9,.9,.9)));
scene.addLight( PointLight(Point(-40, -30, 50), Color(.3,.1,.1)));

scene.setCamera( PinholeCamera(Point(1, -18, 3.5),
                                   Point(1,1, 2),
                                   Vector(0, 0, 1),
                                   90));

