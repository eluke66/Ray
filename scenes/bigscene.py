from Core import *
from Background import *
from Materials import *
from Geometry import *
from IO import *

scene = Scene()
scene.setBackground(ConstantBackground(Color(0.5, 0.8, 0.9)));
wood = phongWoodMaterial(Color(227, 210, 187)/255.,
					 Color(181, 160, 117)/255.,
					 30, ## scale
					 2, ## octaves
					 5, .04, ## ring noise scale, frequency
					 1, .1, ## center noise scale, frequency
					 2, ## lacunarity
					 0.6, ## gain
					 10, ## expon
					 0.5, 0.5, Color(0,0,0), 120);
ice = dielectricMaterial( 1.31, Color(1,1,1), 100 ); 
greenMarble = phongMarbleMaterial(Color(0.0, 1, 0.0), ## Color 1
						   Color(1, 1, 1.0), ## Color 2
						   0.5, ## scale
						   8, ## octaves
						   20, ## turbulence scale
						   1, ## frequency scale
						   2, ## lacunarity
						   0.6, ## gain
						   0.6, 0.4, Color(1,1,1), 120);
  
redMarble = phongMarbleMaterial(Color(1, 1, 1), ## Color 1
						Color(1, 0.2, 0.2), ## Color 2
						0.4, ## scale
						8, ## octaves
						20, ## turbulence scale
						1, ## frequency scale
						2, ## lacunarity
						0.6, ## gain
						0.6, 0.4, Color(1,1,1), 120);
  

chessboardMaterial = checkerMaterial(Point(0,0,0), Vector(0.5,0.5,0), Vector(-0.5,0.5,0), 
			phongMaterial(Color(1, 1, 1), 0.6, 0.4, Color(1,1,1), 60),
			phongMaterial(Color(0, 0, 0), 0.6, 0.4, Color(1,1,1), 60));
world = Group()

world.addObject( Plane( chessboardMaterial,
                        Vector(0,0,1), Point(0,0,2.5)));


world.addObject( Plane( phongMaterial(Color(1,0,0), 0.6, 0.4, Color(1,1,1), 90),
                        Vector(0.3,0.7,0), Point(0,5,0) ) );
world.addObject( Plane( phongMaterial(Color(0,1,0), 0.6, 0.4, Color(1,1,1), 90),
                        Vector(0.7,0.3,0), Point(-8,3,0) ) );


size = Vector(1.0, 1.5, 0.4);
corner = Point( -3.3, -4, 2.5 );
block1 = Box( wood,
              corner,
              corner + size );
world.addObject(block1);

dragon = ObjectLoader.LoadObject( "Objects/dragon_vrip_res4.ply", "" );
p = Position();
p.translate( Vector(-2.8, -2.9, 3.4), True );
p.rotate( Vector(270,-45,0) );
p.scale( 0.3, Position.SCALE_MAX );
p.transform(dragon);
dragon.setMaterial(greenMarble);
world.addObject(dragon);

################ BUDDHA
happy = ObjectLoader.LoadObject( "Objects/happy_vrip_res4.ply", "" );
happy.setMaterial(ice);
p = Position();
p.translate( Vector(-5 , 3, 3.8), True );
p.rotate( Vector(270,-45,0) );
p.scale( 0.5, Position.SCALE_MAX );
p.transform(happy);

world.addObject( happy );

################ Spongecube
spongecube = ObjectLoader.LoadObject( "Objects/spongecube1.aoff", "" );
spongecube.setMaterial(redMarble);
p = Position()
p.translate( Vector(-1, -1, 3), True );
p.scale( 0.5, Position.SCALE_MAX );
p.transform(spongecube);
world.addObject( spongecube );

cone = ObjectLoader.LoadObject( "Objects/cone.aoff", "" );
p = Position()
p.translate( Vector(-3, -3, 8), True );
p.shear( Vector(1,6,1), Position.SHEAR_REL );
p.rotate( Vector(270,0,0) );
p.transform(cone);

world.addObject(cone);

scene.setObject(world);

lightSamples = 50;
scene.addLight( PointLight( Point(-3, -3.5, 4.5 ), Color( .8,.8,.8 ) ) );
scene.addLight( AreaLight(Sphere(None, Point(-1, -1, 3 ), 0.05 ),
                          Color(1,0,0),
                          lightSamples ));

scene.setCamera(PinholeCamera(Point(8, -18, 8.5),
                              Point(-4.7, 2.5, 2.5),
                              Vector(0, 0, 1),
                              15));

scene.setCamera(ThinLensCamera(Point(8, -18, 8.5),
                               Point(-4.7, 2.5, 2.5),
                               Vector(0, 0, 1),
                               15,
                               20, 4.0,
                               40));

scene.setMaxRayDepth(125 );
scene.setMinAttenuation(.01);
scene.setFilter( FilterFactory.findFilter2D( "LanczosSinc2" ) );

