from Core import *
from Background import *
from Materials import *
from Geometry import *
from IO import *
from Filter import *

from time import time

happyCenter = Point(15, 0, -120)

def lightFromPoint(pt):
    happySpot   = Point(happyCenter + Vector(0,70,0))
    lightCenter = Point(happyCenter+pt)
    lightDir    = happySpot - lightCenter
    lightDir.normalize()
    #light       = SimpleSpotLight( lightDir, lightCenter, 12, Color(1e3) )
    light        = SpotLight( lightDir, lightCenter, 1.0, Color(5e2), 7 )
    return light

def makeLights(scene):
    ###
    ###
    ###
    dist = 20
    scene.addLight(lightFromPoint(Point(dist,0,dist)))
    scene.addLight(lightFromPoint(Point(-dist,0,dist)))
    scene.addLight(lightFromPoint(Point(dist/2.,0,dist)))
    scene.addLight(lightFromPoint(Point(-dist/2.,0,dist)))
    scene.addLight(lightFromPoint(Point(0,0,dist)))
    scene.addLight(lightFromPoint(Point(dist,0,0)))
    scene.addLight(lightFromPoint(Point(-dist,0,0)))
    scene.addLight(lightFromPoint(Point(dist/2.0,0,0)))
    scene.addLight(lightFromPoint(Point(-dist/2.0,0,0)))
    ###
    ###
    ###
    power = 100
    height = 15
    scene.addLight( PointLight( Point(0, height, -40), Color(power)) )
    scene.addLight( PointLight( Point(15, height, -40), Color(power)) )
    scene.addLight( PointLight( Point(-15, height, -45), Color(power)) )

def mkHappy():
    g = Group()
    #s = Sphere( basicShade(Color(1.0,0,0)),
    #            Point(0,15,-70), 1 )
    #g.addObject( s )
    #s = Sphere( basicShade(Color(1.0,1.0,0)),
    #            Point(5,15,-60), 1 )
    #g.addObject( s )
    #s = Sphere( basicShade(Color(1.0,0,1.0)),
    #            Point(-5,15,-65), 1 )
    #g.addObject( s )

    
    happy = ObjectLoader.LoadObject( "Objects/happy_vrip_res2.ply", "" )
    happy.setMaterial(lambertianMaterial(Color(1.0,0,0),0.6,0.4))
    #happy.setMaterial(metalMaterial(Color(1.0,0,0), 100 ) )
    p = Position()
    p.translate( Vector(Point(0,44,0) + happyCenter), True )
    p.scale( 10, Position.SCALE_MAX )
    p.transform(happy)

    greenMarble = phongMarbleMaterial(Color(0.0, 0.7, 0.0),
                                      Color(1, 1, 1.0), ## Color 2
                                      0.5, ## scale
                                      8, ## octaves
                                      20, ## turbulence scale
                                      0.5, ## frequency scale
                                      8, ## lacunarity
                                      0.6, ## gain
                                      0.6, 0, Color(1,1,1), 120)
    g.addObject(Box(greenMarble,
                    happyCenter + Point(-10, 0, -10),
                    happyCenter + Point(10, 15, 10) ) )
    g.addObject(happy)
    
    return g

def mkWorship():
    g = GriddedGroup(2)
    drag = ObjectLoader.LoadObject( "Objects/dragon_vrip_res3.ply", "" )
    drag.setMaterial(lambertianMaterial(Color(1.0), 0.6, 0))
    p = Position()
    p.scale(1, Position.SCALE_MAX )
    p.translate( Vector(0,1.5,-50), True )
    p.rotate( Vector(0,90,0) )
    p.transform(drag)
    g.addObject(drag)
    
    centerPoint = Point(0, 0, -120)
    nWorshippers = 10
    xRange = 20
    zRange = 20
    dx = 2 * xRange / nWorshippers
    dz = 2 * zRange / nWorshippers

    for i in range(0,nWorshippers):
        for j in range( 0, nWorshippers ):
            pos = Position()
            v = Vector(-xRange + (i-1)*dx + drand48()*dx,
                       0,
                       -zRange + (j-1)*dx + drand48()*dz )
            pos.translate( v, False )
            c = Color(drand48(), drand48(), drand48())
            lamMat = lambertianMaterial(c, drand48(), 0.5)
            inst = Instance( pos.getMatrix( drag ),
                             drag,
                             lamMat )
            g.addObject(inst)
        
    return g
    
    
world = Group()
groundMat = lambertianMaterial(Color(0.5),0.6, 0)
groundMat1 = checkerMaterial( Point(0,0,0), Vector(0.5,0.5,0), Vector(-0.5,0.5,0),
                             lambertianMaterial(Color(0.0),0.6, 0),
                             lambertianMaterial(Color(1.0),0.6, 0) )
world.addObject(Plane(groundMat, Vector(0,1,0), Point(0,0,0) ) )
world.addObject(mkHappy())
world.addObject(mkWorship())



camEye = Point(0, 0.2, 0)
camAt  = Point(0, 20, -100 )
camUp  = Vector(0,1,0)
FOV    = 37

camera = PinholeCamera( camEye, camAt, camUp, FOV )
#camera = ThinLensCamera( camEye, camAt, camUp, FOV,
#                         50, 4, 10 )

scene = Scene()
makeLights(scene)
#scene.setBackground( ConstantBackground(Color(0,0,0)) )
scene.setBackground( StarfieldBackground( Color(0.0) ) )
scene.setMaxRayDepth(10)
scene.setMinAttenuation(.01)
scene.setAmbient(Color(0.))
scene.setObject( world )
scene.setCamera( camera )


m = PinholeCameraMover()
#m.endEye( Point(0,10,0) )
m.addPoint( Point( 0, 10, 0 ), True, Point(0.0,0.0,0.0), False, Vector(0.0,0.0,0.0), False )
m.addPoint( Point( 0, 5, -20 ), True, Point(0.0,0.0,0.0), False, Vector(0.0,0.0,0.0), False )
m.addPoint( Point( 0, 0, -40 ), True, Point(0.0,0.0,0.0), False, Vector(0.0,0.0,0.0), False )
scene.addFrameManipulator(m)
