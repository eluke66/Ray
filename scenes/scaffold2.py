from Core import *
from Background import *
from Materials import *
from Geometry import *
from IO import *
from Filter import *
import math
import scenes


def makeScaffold(box,angle,top,bottom,scaffThick,supportThickness,x,y,forwardSlant):
    print "AngleCos = ",math.cos(math.radians(90-angle))
    print "BaseHeight = ",(top-bottom)/math.cos(math.radians(90-angle))
    print "Thickness Addition = ", (2.0*thick / math.tan(math.radians(angle)))
    scaffHeight = (top-bottom)/math.cos(math.radians(90-angle)) + 2.0*supportThickness / math.tan(math.radians(angle))
    print "HEIGHT: ",scaffHeight
    dx = 0.5*scaffHeight*math.cos(math.radians(angle))
    print "DX is ",dx
    
    base = 0#-scaffHeight*0.5
    box = Box(scafMat,
              Point(-sThick*0.5,
                    -sThick*0.5,
                    base),
              Point(sThick*0.5,
                    sThick*0.5,
                    base+scaffHeight))
    
    
    vOffset = thick * math.sin(math.radians(90-angle));
    print "V offset is ",vOffset
    print "Top,bottom",top,bottom
    zTranslate = bottom - vOffset + (top-bottom+thick) * 0.5 
    p = Position()
    if ( forwardSlant ):
        p.rotate( Vector(0, 90+angle, 0) )
    else:
        p.rotate( Vector(0, 90-angle, 0) )
    p.translate( Vector( x, y, zTranslate ), True )
    return Instance( p.getMatrix(box),
                     box,
                     scafMat )
    

scene = Scene()
world = Group();
scene.setBackground( ConstantBackground( Color (.745, 0.462, 0.313 ) ) )

frameMat = lambertianMaterial( Color( 1, 1, 0 ), 0.6, 0.4 )
scafMat  = lambertianMaterial( Color( 1, 0, 0 ), 0.6, 0.4 )


width = 10.0
top = 10.0
bottom = 0.0
thick = 1.0
sThick = 1.0
xMin = -10.0
xMax = 10.0
yMin = 0.0
yMax = yMin + thick

angle = 60 # degrees

# TOP
world.addObject( Box( frameMat,
                      Point(xMin, yMin, top),
                      Point(xMax, yMax, top+thick) ) )
# BOTTOM
world.addObject( Box( frameMat,
                      Point(xMin, yMin, bottom-thick),
                      Point(xMax, yMax, bottom) ) )


print "AngleCos = ",math.cos(math.radians(90-angle))
print "BaseHeight = ",(top-bottom)/math.cos(math.radians(90-angle))
print "Thickness Addition = ", (2.0*thick / math.tan(math.radians(angle)))
scaffHeight = (top-bottom)/math.cos(math.radians(90-angle)) + 2.0*thick / math.tan(math.radians(angle))
print "HEIGHT: ",scaffHeight
dx = 0.5*scaffHeight*math.cos(math.radians(angle))
print "DX is ",dx
stride = thick+2.0*dx

base = 0#-scaffHeight*0.5
box = Box(scafMat,
          Point(-sThick*0.5,
                -sThick*0.5,
                base),
          Point(sThick*0.5,
                sThick*0.5,
                base+scaffHeight))

for i in range(0,16,1):
    db = 0.01 * thick*0.5+dx+i*stride
    world.addObject(makeScaffold(box,angle, top, bottom-db, sThick, thick, xMin+thick*0.5+dx+i*stride, yMin+(yMax-yMin) / 2.0, True))


for i in range(0,16,1):
    db = 0.01 * thick*0.5+dx+i*stride
    world.addObject(makeScaffold(box,angle, top, bottom-db, sThick, thick, xMin+thick*0.5+dx+i*stride, yMin+(yMax-yMin) / 2.0, False))
    




scene.setObject( world );

scene.addLight(PointLight(Point(0, -30, 10), Color(1500)));

scene.setCamera(PinholeCamera(Point(0, -600, 5.5),
                              Point(0, 0, 5),
                              Vector(0, 0, 1),
                              50))
scene.setMaxRayDepth(10);
scene.setMinAttenuation(.01);
