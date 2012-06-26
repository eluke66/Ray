from Core import *
from Background import *
from Materials import *
from Geometry import *
from IO import *
from Filter import *

# Just a primitive
def makeCG1():
  b = Box( lambertianMaterial( Color(1,0,0), 0.6, 0.4 ),
           Point( -10, -10, -10 ),
           Point( 10, 10, 10 ) )

  p = CSG_Primitive(b)
  c = CSG(None)
  c.setCSG(p)
  return c

# Primitive OR primitive
def makeCG2():
  b = Box( lambertianMaterial( Color(1,0,0), 0.6, 0.4 ),
           Point( -10, -10, -10 ),
           Point( 10, 10, 10 ) )
  p1 = CSG_Primitive(b)
  s = Sphere( lambertianMaterial( Color(0,1,0), 0.6, 0.4 ),
              Point( 0,0,0 ), 11 )
  p2 = CSG_Primitive(s)
  union = CSG_Or(p1, p2)
  
  c = CSG(None)
  c.setCSG(union)
  return c

# Primitive AND primitive
def makeCG3():
  b = Box( lambertianMaterial( Color(1,0,0), 0.6, 0.4 ),
           Point( -10, -10, -10 ),
           Point( 10, 10, 10 ) )
  p1 = CSG_Primitive(b)
  s = Sphere( lambertianMaterial( Color(0,1,0), 0.6, 0.4 ),
              Point( 0,0,0 ), 11 )
  p2 = CSG_Primitive(s)
  union = CSG_And(p1, p2)
  
  c = CSG(None)
  c.setCSG(union)
  return c

# Primitive AND NOT primitive
def makeCG4():
  b = Box( lambertianMaterial( Color(1,0,0), 0.6, 0.4 ),
           Point( -10, -10, -10 ),
           Point( 10, 10, 10 ) )
  p1 = CSG_Primitive(b)
  s = Sphere( lambertianMaterial( Color(0,1,0), 0.6, 0.4 ),
              Point( 0,0,0 ), 11 )
  p2 = CSG_Primitive(s)
  union = CSG_And(CSG_Not(p1), p2)
  
  c = CSG(None)
  c.setCSG(union)
  return c

# Primitive AND (NOT primitive AND primitive)
def makeCG4():
  b = Box( lambertianMaterial( Color(1,0,0), 0.6, 0.4 ),
           Point( -10, -10, -10 ),
           Point( 10, 10, 10 ) )
  p1 = CSG_Primitive(b)
  s = Sphere( lambertianMaterial( Color(0,1,0), 0.6, 0.4 ),
              Point( 0,0,0 ), 11 )
  p2 = CSG_Primitive(s)
  
  pl = Plane( lambertianMaterial( Color(0,0,1), 0.6, 0.4 ),
              Vector( 0,0,1), Point( 0,0,0 ) )
  p3 = CSG_Primitive(pl)

  union = CSG_And(p1, CSG_And(CSG_Not(p2), p3))
  
  c = CSG(None)
  c.setCSG(union)
  return c

# Primitive AND (NOT primitive AND primitive)
def makeWoodenBowl():
  wood = phongWoodMaterial(Color(227, 210, 187)/255.,
                           Color(181, 160, 117)/255.,
                           30, 2,  5, .04, 1, .1, 2, 0.6, 10,
                           0.5, 0.5, Color(0,0,0), 120)


  inner = Sphere( wood,
                  Point( 0,0,0 ), 9 )
  outer = Sphere( wood,
                  Point( 0,0,0 ), 12 )
  
  pl = Plane( phongMaterial(Color(1,1,1), 0.9, 0.4, Color(1,1,1), 100 ),#wood,
              Vector( 0,-0.5,1), Point( 0,0,0 ) )

  c = CSG(None)
  c.setCSG( CSG_And( CSG_Primitive(outer),
                     CSG_And(CSG_Not(CSG_Primitive(inner)),
                             CSG_Primitive(pl))) )

  return c

# Primitive AND (NOT primitive AND primitive)
def makeCrystalBowl():
  
  glass = dielectricMaterial( 1.333, Color(1,0,1), 100 )
  glass1 = lambertianMaterial( Color(1,1,1), 0.6, 0.4 )

  inner = Sphere( glass,
                  Point( 0,0,0 ), 9 )
  outer = Sphere( glass1,
                  Point( 0,0,0 ), 12 )
  
  pl = Plane( glass,
              Vector( 0,-0.5,1), Point( 0,0,0 ) )

  c = CSG(None)
  c.setCSG( CSG_And( CSG_Primitive(outer),
                     CSG_And(CSG_Not(CSG_Primitive(inner)),
                             CSG_Primitive(pl))) )

  floorMatl = checkerMaterial(Point(0,0,0), Vector(0.5,0.5,0), Vector(-0.5,0.5,0),
                              phongMaterial(Color(1, 1, 1), 1., 0.4, Color(1,1,1), 60),
                              phongMaterial(Color(0, 0, 0), 0.6, 0.4, Color(1,1,1), 60))

  floor = Plane( floorMatl, Vector(0,0,1), Point(0,0,-20) )
                        
  grp = Group()
  grp.addObject(c)
  grp.addObject(floor)
  return grp


scene = Scene()
world = Group();
scene.setBackground( GradientBackground( Color (.745, 0.462, 0.313 ),
                                             Color (0,0,1),
                                             False, 3.0 ) )

#g = makeCG1()
#g = makeCG2()
#g = makeCG3()
#g = makeCG4()
g = makeWoodenBowl()
#g = makeCrystalBowl()

world.addObject( g )
scene.setObject( world );

scene.addLight(PointLight(Point(0, -20, 40), Color(2000.9)));


scene.setCamera(PinholeCamera(Point(0, -60, 22.5),
                              Point(0, 0, 0),
                              Vector(0, 0, 1),
                              55));
scene.setMaxRayDepth(10);
scene.setMinAttenuation(.01);

