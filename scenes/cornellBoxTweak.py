from Core import *
from Background import *
from Materials import *
from Geometry import *
from IO import *
from Filter import *

Ka = 0.4
if ( True ):
    Ka = 0.0
    
whiteMatl = lambertianMaterial(Color(1.0), 0.6, Ka)

def makeFloor():
    #floorMatl = whiteMatl
    bc = BrickColor( Color(1,0,0), Color(0.1,0,0),
                     True, Point2D(0.8,0.8),
                     Color(1,1,1),25,10 )
    floorMatl = checkerMaterial(Point(0,0,0),
                                Vector(0.5,0.5,0),
                                Vector(-0.5,0.5,0),
                                metalMaterial(Color(1,1,0), 1000),
                                lambertianBrickMaterial(bc,0.6,0))
                                #lambertianMaterial(Color(1.0), 0.6, Ka))
    floor = Box( floorMatl,
                 Point(0,-5,0),
                 Point(552.8,0,559.2) )
    return floor

def makeCeiling():
    cMatl = whiteMatl
    ceil = Box( cMatl,
                Point(0,548.8,0),
                Point(556,549,559.2) )
    return ceil
                 
def makeLight():
    lightP = BoundedPlane( basicShade(Color(1)),
                           Vector(0,-1,0), Point(0,548.8,0),
                           Point(213.0, 548.79, 227),
                           Point(343, 548.81, 332 ) )
    lCol = Color(10.0)
    light = PlaneLight( lightP,
                        lCol * 3e4,
                        200 )
    return light

def makeBack():
    matl = whiteMatl
    return Box( matl,
                Point(0,  0,     559.2),
                Point(556,548.8, 559.5) )

def makeRight():
    global Ka
    matl = lambertianMaterial(Color(0,0,1), 0.6, Ka)
    return Box( matl,
                Point(-1, 0,     0),
                Point(0,  548.8, 559.2) )

def makeLeft():
    global Ka
    matl = lambertianMaterial(Color(1,0,0), 0.6, Ka)
    return Box( matl,
                Point(549,  0,    0),
                Point(556,548.8, 559.2) )

def makeQuad(g,mat,p1,p2,p3,p4):
    g.addObject(Triangle(mat, p1,p2,p3 ))
    g.addObject(Triangle(mat, p3, p4, p1))
    
def makeShortBlock():
    block = Group()
    #mat = whiteMatl
    #mat = dielectricMaterial(2.333, Color(1,0.1,1), 1001)
    mat = metalMaterial(Color(1), 1001)
    makeQuad(block,mat,Point(130.0, 165.0,  65.0),
             Point(82.0, 165.0, 225.0),
             Point(240.0, 165.0, 272.0),
             Point(290.0, 165.0, 114.0))
    
    makeQuad(block,mat,Point(290.0,   0.0, 114.0),
             Point(290.0, 165.0, 114.0),
             Point(240.0,165.0, 272.0),
             Point(240.0 ,  0.0, 272.0))
                             
    makeQuad(block,mat,Point(130.0,   0.0,  65.0),
             Point(130.0, 165.0,  65.0),
             Point(290.0, 165.0, 114.0),
             Point(290.0,   0.0, 114.0))

    makeQuad( block,mat,Point(82.0 ,  0.0, 225.0),
              Point(82.0, 165.0, 225.0),
              Point(130.0, 165.0,  65.0),
              Point(130.0,   0.0,  65.0))
    
    makeQuad(block,mat,Point(240.0 ,  0.0, 272.0),
             Point(240.0, 165.0, 272.0),
             Point(82.0, 165.0, 225.0),
             Point(82.0,   0.0, 225.0))
    return block


def makeTallBlock():
    block = Group()
    mat = whiteMatl

    makeQuad( block,mat,Point(423.0, 330.0 ,247.0),
              Point(265.0, 330.0, 296.0),
              Point(314.0, 330.0, 456.0),
              Point(472.0, 330.0, 406.0))

    makeQuad( block,mat,Point(423.0,   0.0, 247.0),
              Point(423.0, 330.0, 247.0),
              Point(472.0, 330.0, 406.0),
              Point(472.0,   0.0, 406.0))

    makeQuad( block,mat,Point(472.0,   0.0, 406.0),
              Point(472.0, 330.0, 406.0),
              Point(314.0, 330.0, 456.0),
              Point(314.0,   0.0, 456.0))

    makeQuad( block,mat,Point(314.0,   0.0, 456.0),
              Point(314.0, 330.0, 456.0),
              Point(265.0, 330.0, 296.0),
              Point(265.0,   0.0, 296.0))

    makeQuad( block,mat,Point(265.0,   0.0, 296.0),
              Point(265.0, 330.0, 296.0),
              Point(423.0, 330.0, 247.0),
              Point(423.0,   0.0, 247.0))
              
    return block

ballRadius = 70.0
def makeMirrorBall():
    mat = metalMaterial(Color(1), 1001)
    radius = ballRadius
    return Sphere( mat,
                   Point(335+radius/2,
                         radius,
                         247+radius/2),
                   radius )
    
def makeGlassBall():
    g = Group()
    mat = dielectricMaterial(1.333, Color(1), 1001)
    radius = ballRadius / 3.0
    g.addObject(Sphere( mat,
                        Point(72+radius/2,
                              radius,
                              85+radius/2),
                        radius ))
    g.addObject(Sphere( mat,
                        Point(102+3*radius/2,
                              165+radius,
                              115+radius/2),
                        radius ))
    
    g.addObject(Sphere( mat,
                        Point(112+5*radius/2,
                              165+radius,
                              45+radius/2),
                        radius ))
    g.addObject(Sphere( mat,
                        Point(12+5*ballRadius/2,
                              165+ballRadius,
                              245+ballRadius/2),
                        ballRadius ))
    return g
    
world = Group()
world.addObject(makeFloor())
world.addObject(makeCeiling())
world.addObject(makeBack())
world.addObject(makeRight())
world.addObject(makeLeft())

#blocks = False
#if ( blocks ):
world.addObject(makeShortBlock())
world.addObject(makeTallBlock())
#else:
world.addObject(makeMirrorBall())
world.addObject(makeGlassBall())

world.addObject(BoundedPlane( basicShade(Color(1)),
                              Vector(0,-1,0), Point(0,548.799,0),
                              Point(213.0, 548.79, 227),
                              Point(343, 548.81, 332 ) ) )

camera = PinholeCamera( Point(278, 273, -800),
                        Point(278, 273, 0 ),
                        Vector(0,1,0),
                        37 ) # This is a guess


scene = Scene()
scene.addLight( makeLight() )
scene.setBackground( ConstantBackground(Color(0,0,0)) )
scene.setMaxRayDepth(10)
scene.setMinAttenuation(.01)
scene.setAmbient(Color(0.1))
scene.setObject( world )
scene.setCamera( camera )


