from Core import *
from Background import *
from Materials import *
from Geometry import *
from IO import *

scene = Scene()
scene.setBackground(ConstantBackground(Color(0.5, 0.8, 0.9)));
woodp = phongWoodMaterial(Color(217, 200, 177)/255.,
					 Color(171, 150, 107)/255.,
					 30, ## scale
					 2, ## octaves
					 5, .04, ## ring noise scale, frequency
					 1, .1, ## center noise scale, frequency
					 2, ## lacunarity
					 0.6, ## gain
					 10, ## expon
					 0.5, 0.5, Color(0,0,0), 120);
wood = lambertianWoodMaterial(Color(217, 200, 177)/255.,
					 Color(171, 150, 107)/255.,
					 30, ## scale
					 2, ## octaves
					 5, .04, ## ring noise scale, frequency
					 1, .1, ## center noise scale, frequency
					 2, ## lacunarity
					 0.6, ## gain
					 10, ## expon
					 0.5, 0.5)
ice = dielectricMaterial( 1.31, Color(0.6,1.0,0.2), 100 ); 
greenMarble = phongMarbleMaterial(Color(0.0, 1, 0.0), ## Color 1
						   Color(1, 1, 1.0), ## Color 2
						   0.5, ## scale
						   8, ## octaves
						   20, ## turbulence scale
						   1, ## frequency scale
						   2, ## lacunarity
						   0.6, ## gain
						   0.6, 0.4, Color(1,1,1), 120);

darkGreenMarble = phongMarbleMaterial(Color(0.0, 0.15, 0.0), ## Color 1
                                      Color(1.0), ## Color 2
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
                                     #lambertianMaterial(Color(1.0), 0.6, 0.4),
                                     lambertianReflectingMaterial(Color(1.0), 0.6, 0.4, 0.8),
                                     #phongMaterial(Color(0.0), 0.6, 0.4, Color(1,1,1), 60))
                                     phongReflectingMaterial(Color(0.0), 0.6, 0.4, Color(1,1,1), 60,0.8))
world = Group()

#world.addObject( Plane( darkGreenMarble,#chessboardMaterial,
#                        Vector(0,0,1), Point(0,0,2.5)));
extent = 7
world.addObject( BoundedPlane( darkGreenMarble, Vector(0,0,1), Point(0,0,2.5),
                               Point(-extent,-extent,2.4999), Point( extent, extent, 2.5001) ) )

world.addObject( Plane( phongMaterial(Color(1,0,0), 0.6, 0.4, Color(1,1,1), 90),
                        Vector(0.3,0.7,0), Point(0,5,0) ) );
world.addObject( Plane( phongMaterial(Color(0,1,0), 0.6, 0.4, Color(1,1,1), 90),
                        Vector(0.7,0.3,0), Point(-8,3,0) ) );


################ BUDDHA
happy = ObjectLoader.LoadObject( "Objects/happy_vrip_res3.ply", "" );
mod = MaterialModulator( SplitModulator(0.153,happy,2), [wood, ice])
happy.setMaterial(mod)
#happy.setMaterial(redMarble)#happy.setMaterial(ice);
p = Position();
p.translate( Vector(-2, -1, 3.8), True );
p.rotate( Vector(270,-45,0) );
p.scale( 0.5, Position.SCALE_MAX );
p.transform(happy);

world.addObject( happy );

scene.setObject(world);

lightSamples = 15;
scene.addLight( PointLight( Point(-3, -3.5, 4.5 ), Color( 5.8 ) ) );
scene.addLight( PointLight( Point(-5, 2.5, 4.5 ), Color( 2,2,3 ) ) );

scene.addLight( SphereLight(Sphere(None, Point(-1, -1, 3 ), 0.05 ),
                            Color(0.5,.1,.1),
                            lightSamples ));

scene.setCamera(PinholeCamera(Point(8, -18, 8.5),
                              Point(-4.7, 2.5, 2.5),
                              Vector(0, 0, 1),
                              15));

#scene.setCamera(ThinLensCamera(Point(8, -18, 8.5),
#                               Point(-4.7, 2.5, 2.5),
#                               Vector(0, 0, 1),
#                               15,
#                               20, 4.0,
#                               40));

scene.setMaxRayDepth(125 );
scene.setMinAttenuation(.01);

