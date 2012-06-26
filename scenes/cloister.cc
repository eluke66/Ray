#include "all.h"
#include "Core/Position.h"

Scene* cloister()
{
  Scene* scene = new Scene();
  scene->setBackground(new ConstantBackground(Color(.1, .2, .3)));
  Group* world = new Group();
  Object * cloister = ObjectLoader::LoadObject( "/home/luke/Desktop/Models/Models/Architecture/Cloister.3ds", "" );

  Material* disk2matl = new LambertianMaterial(Color(.2, 1, .2), .6, .4);
  cloister->setMaterial(disk2matl, false);
  Position p;
  p.translate( Vector(0,0,0), true );
  p.rotate( Vector(0,0,180) );
  p.scale( 4, Position::SCALE_MAX );
  p.transform(cloister);

  
  world->addObject(cloister);
  scene->setObject(world);

  //scene->setAmbient(Color(.1, .1, .1));
  scene->addLight(new PointLight(Point(20, 30, 100), Color(.9,.9,.9)));
  scene->addLight(new PointLight(Point(0, -20, 0), Color(.1,.1,.1)));
  scene->addLight(new PointLight(Point(-40, -30, 50), Color(.3,.1,.1)));

  scene->addLight( new PointLight( Point( -3.56164, -6.57069, -6.20762), Color(0.0010019, 8.51953e-05, 8.51953e-05) )); 
scene->addLight( new PointLight( Point( -7.83665, -6.73724, -3.2118), Color(0.0229853, 0.0211162, 0.0196412) )); 
scene->addLight( new PointLight( Point( -2.27026, -3.81434, -0.241376), Color(0.00147204, 0.00129281, 0.000981231) )); 
scene->addLight( new PointLight( Point( 0.988449, 3.14237, 1.61185), Color(0.0525393, 0.0225092, 0.0140829) )); 
scene->addLight( new PointLight( Point( -4.45521, -2.77256, 4.55137), Color(0.00148737, 0.000522805, 0.000321648) )); 
scene->addLight( new PointLight( Point( -4.15064, -2.44808, 4.14869), Color(0.00728235, 0.0058981, 0.0035509) )); 
scene->addLight( new PointLight( Point( -4.60128, -3.43813, 2.89439), Color(0.0384679, 0.0143173, 0.00861963) )); 
scene->addLight( new PointLight( Point( -4.60128, -2.9025, 4.54146), Color(0.0389402, 0.0144449, 0.00869639) )); 
scene->addLight( new PointLight( Point( -5.72107, -4.55535, 2.56129), Color(0.000695196, 0.00169199, 0.000338399) )); 
scene->addLight( new PointLight( Point( 6.46527, -5.45201, 1.79075), Color(0.00533333, 0.0266667, 0.00533333) )); 
scene->addLight( new PointLight( Point( -5.76887, -4.46, 2.40689), Color(0.00533333, 0.0266667, 0.00533333) )); 
scene->addLight( new PointLight( Point( 7.95277, -6.56684, -6.20762), Color(0.000931935, 8.07613e-05, 8.07613e-05) )); 
scene->addLight( new PointLight( Point( 3.41877, -6.9195, -0.554732), Color(0.00533333, 0.0266667, 0.00533333) )); 
scene->addLight( new PointLight( Point( 5.91332, 6.78272, -1.1381), Color(0.0233322, 0.0211913, 0.0198881) )); 
scene->addLight( new PointLight( Point( -6.02902, 4.44124, 2.40689), Color(0.000664948, 0.00164158, 0.000328316) )); 
scene->addLight( new PointLight( Point( -5.15408, 5.06684, 2.13376), Color(0.00533333, 0.0266667, 0.00533333) )); 
scene->addLight( new PointLight( Point( -5.43227, 4.06402, 2.17672), Color(0.00533333, 0.0266667, 0.00533333) )); 
scene->addLight( new PointLight( Point( -5.42011, 4.06402, 2.21577), Color(0.00533333, 0.0266667, 0.00533333) )); 
scene->addLight( new PointLight( Point( -8.16114, 4.94808, 2.55819), Color(0.00316676, 0.00137436, 0.00125467) )); 
scene->addLight( new PointLight( Point( -2.39133, 3.14237, 2.48965), Color(0.00146696, 0.00063667, 0.000379955) )); 
scene->addLight( new PointLight( Point( -2.36483, 3.17467, 2.50354), Color(0.0145723, 0.0118879, 0.00709439) )); 
scene->addLight( new PointLight( Point( -6.62175, -1.99968, -6.20762), Color(0.00581173, 0.00109952, 0.00109952) )); 
scene->addLight( new PointLight( Point( 6.1943, -7.45916, 1.75336), Color(0.000390124, 0.00118354, 0.000236708) )); 
scene->addLight( new PointLight( Point( 6.46527, -7.45868, 2.09279), Color(0.00533333, 0.0266667, 0.00533333) )); 
scene->addLight( new PointLight( Point( 6.46527, -7.35095, 1.6329), Color(0.00533333, 0.0266667, 0.00533333) )); 
scene->addLight( new PointLight( Point( 3.31324, -5.27435, 2.56129), Color(0.0218741, 0.0542345, 0.0108469) )); 
scene->addLight( new PointLight( Point( -7.28428, 3.23095, 2.5254), Color(0.00199607, 0.000782222, 0.000470471) )); 
scene->addLight( new PointLight( Point( -7.30737, 3.2598, 2.40689), Color(0.00533333, 0.0266667, 0.00533333) )); 
scene->addLight( new PointLight( Point( -7.27556, 3.14237, 2.52291), Color(0.0502176, 0.021581, 0.0128791) )); 
scene->addLight( new PointLight( Point( -7.12106, 3.14237, 2.51286), Color(0.0501721, 0.0215685, 0.0128716) )); 
scene->addLight( new PointLight( Point( -8.37386, -1.04816, 1.81973), Color(0.00239285, 0.00113422, 0.00102688) )); 
scene->addLight( new PointLight( Point( -7.83665, -0.827787, 2.12007), Color(0.0234176, 0.0211712, 0.0195205) )); 
scene->addLight( new PointLight( Point( -7.83665, -0.894304, 1.95067), Color(0.0233467, 0.0211003, 0.0193449) )); 
scene->addLight( new PointLight( Point( -8.10092, -0.45222, 2.13376), Color(0.00533333, 0.0266667, 0.00533333) )); 
scene->addLight( new PointLight( Point( -7.44846, 0.171721, 2.40689), Color(0.00068656, 0.0016776, 0.00033552) )); 
scene->addLight( new PointLight( Point( -6.84945, 0.138761, 2.13376), Color(0.00533333, 0.0266667, 0.00533333) )); 
scene->addLight( new PointLight( Point( -7.74339, 0.200496, 2.13376), Color(0.00533333, 0.0266667, 0.00533333) )); 
scene->addLight( new PointLight( Point( -7.10744, 0.0213566, 2.13376), Color(0.00533333, 0.0266667, 0.00533333) )); 
scene->addLight( new PointLight( Point( 8.11208, -2.35331, 1.06742), Color(0.000636209, 0.00159368, 0.000318736) )); 
scene->addLight( new PointLight( Point( 6.44482, -2.69051, 1.59515), Color(0.00728235, 0.0058981, 0.0035509) )); 
scene->addLight( new PointLight( Point( 6.46527, -3.71016, 1.31861), Color(0.00533333, 0.0266667, 0.00533333) )); 
scene->addLight( new PointLight( Point( 5.65748, 6.52701, -6.20762), Color(0.000898353, 7.86432e-05, 7.86432e-05) )); 
scene->addLight( new PointLight( Point( -7.83665, 5.83757, -1.05263), Color(0.0236829, 0.021363, 0.0201038) )); 
scene->addLight( new PointLight( Point( -7.83665, 6.58001, -1.63575), Color(0.0239262, 0.0216135, 0.0203787) )); 
scene->addLight( new PointLight( Point( 9.04178, -5.69921, 0.681217), Color(0.00533333, 0.0266667, 0.00533333) )); 
scene->addLight( new PointLight( Point( -2.27384, -5.41854, -6.20762), Color(0.000989735, 8.4417e-05, 8.4417e-05) )); 
scene->addLight( new PointLight( Point( -0.65834, -0.95317, -5.60045), Color(0.223127, 0.224956, 0.0308575) )); 
scene->addLight( new PointLight( Point( -7.83665, -5.8802, -1.59762), Color(0.0229629, 0.0211982, 0.019265) )); 
scene->addLight( new PointLight( Point( -8.1107, 4.6552, 2.55819), Color(0.0030465, 0.00133879, 0.00123196) )); 
scene->addLight( new PointLight( Point( -7.96583, 4.49858, 2.40689), Color(0.00533333, 0.0266667, 0.00533333) )); 
scene->addLight( new PointLight( Point( -7.83665, 4.61515, 2.46871), Color(0.0233643, 0.0211567, 0.0194883) )); 
scene->addLight( new PointLight( Point( -7.96779, 4.28245, 2.40689), Color(0.00533333, 0.0266667, 0.00533333) )); 
scene->addLight( new PointLight( Point( -4.25048, -1.64456, 4.55137), Color(0.00146956, 0.000517591, 0.000317658) )); 
scene->addLight( new PointLight( Point( -4.15064, -1.68587, 4.49413), Color(0.00728235, 0.0058981, 0.0035509) )); 
scene->addLight( new PointLight( Point( -4.57369, -0.75277, 2.53928), Color(0.00728235, 0.0058981, 0.0035509) )); 
scene->addLight( new PointLight( Point( 5.69364, 6.63602, 2.40689), Color(0.000614069, 0.00155678, 0.000311356) )); 
scene->addLight( new PointLight( Point( 5.75141, 5.82353, 2.13376), Color(0.00533333, 0.0266667, 0.00533333) )); 
scene->addLight( new PointLight( Point( 4.00108, 3.59181, 3.40647), Color(0.0145723, 0.0118879, 0.00709439) )); 
scene->addLight( new PointLight( Point( -2.71814, 3.14116, 2.92371), Color(0.00147408, 0.000638607, 0.000381111) )); 
scene->addLight( new PointLight( Point( -2.60864, 3.20884, 2.52857), Color(0.0145723, 0.0118879, 0.00709439) )); 
scene->addLight( new PointLight( Point( -1.89005, 3.59181, 2.54227), Color(0.0145723, 0.0118879, 0.00709439) )); 
scene->addLight( new PointLight( Point( 3.26937, -2.02803, 3.27447), Color(0.135365, 0.193731, 0.178631) )); 
scene->addLight( new PointLight( Point( -2.69742, -5.12138, -6.20762), Color(0.000991084, 8.45031e-05, 8.45031e-05) )); 
scene->addLight( new PointLight( Point( -7.83665, -6.3364, -4.64426), Color(0.0232198, 0.0209735, 0.0192154) )); 
scene->addLight( new PointLight( Point( 5.98149, -2.4713, 0.4125), Color(0.00728235, 0.0058981, 0.0035509) )); 
scene->addLight( new PointLight( Point( -7.83665, -6.46855, -6.07701), Color(0.0231099, 0.0209606, 0.0189113) )); 
scene->addLight( new PointLight( Point( 11.5654, -4.49595, 1.06742), Color(0.000626495, 0.00157749, 0.000315498) )); 
scene->addLight( new PointLight( Point( 6.46527, -3.70907, 2.12885), Color(0.00533333, 0.0266667, 0.00533333) )); 
scene->addLight( new PointLight( Point( 10.8484, -4.53479, 0.681217), Color(0.00533333, 0.0266667, 0.00533333) )); 
scene->addLight( new PointLight( Point( 6.46527, -3.86638, 2.00938), Color(0.00533333, 0.0266667, 0.00533333) )); 
scene->addLight( new PointLight( Point( -8.37386, 0.157265, -0.136438), Color(0.000997977, 0.000272397, 0.000160717) )); 
scene->addLight( new PointLight( Point( -7.83665, 0.380792, -0.0884236), Color(0.0234239, 0.0213431, 0.0200471) )); 
scene->addLight( new PointLight( Point( -7.83665, -0.529815, -0.374281), Color(0.0234007, 0.0211813, 0.0194098) )); 
scene->addLight( new PointLight( Point( -7.83665, 0.286746, 0.0176499), Color(0.0233977, 0.0212924, 0.0196402) )); 
scene->addLight( new PointLight( Point( -8.37386, 2.11989, 0.705485), Color(0.00181491, 0.000724661, 0.000386) )); 
scene->addLight( new PointLight( Point( -7.90409, 1.38204, 2.13376), Color(0.00533333, 0.0266667, 0.00533333) )); 
scene->addLight( new PointLight( Point( -7.83665, 2.08133, 0.600669), Color(0.0232283, 0.020982, 0.019224) )); 
scene->addLight( new PointLight( Point( -7.83665, 2.41239, 1.07926), Color(0.0234186, 0.0213039, 0.0199365) )); 
scene->addLight( new PointLight( Point( 2.61355, -0.536391, 1.4262), Color(0.00248193, 0.00238086, 0.000198066) )); 
scene->addLight( new PointLight( Point( 3.11889, -0.00603759, 0.778318), Color(0.137684, 0.0997553, 0.062056) )); 
scene->addLight( new PointLight( Point( -3.13344, 3.14116, 2.70755), Color(0.0145723, 0.0118879, 0.00709439) )); 
scene->addLight( new PointLight( Point( 1.91064, -1.5161, 1.00805), Color(0.0452214, 0.0654275, 0.0078881) )); 
scene->addLight( new PointLight( Point( -4.60128, -0.773938, 2.98718), Color(0.0011398, 0.000425741, 0.000256452) )); 
scene->addLight( new PointLight( Point( -4.21666, -0.93485, 2.53916), Color(0.00728235, 0.0058981, 0.0035509) )); 
scene->addLight( new PointLight( Point( -4.17648, -1.15872, 2.53901), Color(0.00728235, 0.0058981, 0.0035509) )); 
scene->addLight( new PointLight( Point( -4.87829, -0.918345, 2.40689), Color(0.0226631, 0.0555495, 0.0111099) )); 
scene->addLight( new PointLight( Point( 0.117333, -4.59089, 2.56129), Color(0.000668857, 0.0016481, 0.000329619) )); 
scene->addLight( new PointLight( Point( -2.27919, 3.14116, 3.28022), Color(0.0949384, 0.0365543, 0.0279709) )); 
scene->addLight( new PointLight( Point( 11.3522, 2.92877, 1.06742), Color(0.000610511, 0.00155085, 0.00031017) )); 
scene->addLight( new PointLight( Point( 10.0901, 2.25253, 0.681218), Color(0.00533333, 0.0266667, 0.00533333) )); 
scene->addLight( new PointLight( Point( 6.44482, 0.858152, 0.91602), Color(0.00728235, 0.0058981, 0.0035509) )); 
scene->addLight( new PointLight( Point( 10.0351, 3.20216, 0.898568), Color(0.0145723, 0.0118879, 0.00709439) )); 
scene->addLight( new PointLight( Point( -5.01915, 4.97485, 2.40689), Color(0.000659199, 0.001632, 0.0003264) )); 
scene->addLight( new PointLight( Point( -5.42919, 4.06402, 2.25062), Color(0.00533333, 0.0266667, 0.00533333) )); 
scene->addLight( new PointLight( Point( -4.60128, 4.98998, 2.6817), Color(0.103109, 0.0359805, 0.03048) )); 
scene->addLight( new PointLight( Point( -4.60128, 2.7383, 2.77948), Color(0.0384481, 0.0143943, 0.00883829) )); 
scene->addLight( new PointLight( Point( 1.71308, -5.15206, 2.56129), Color(0.000663173, 0.00163862, 0.000327724) )); 
scene->addLight( new PointLight( Point( -3.3289, -5.41327, -0.159333), Color(0.00533333, 0.0266667, 0.00533333) )); 
scene->addLight( new PointLight( Point( 3.56423, -3.04427, 2.68219), Color(0.135365, 0.193731, 0.178631) )); 
scene->addLight( new PointLight( Point( 2.14807, -3.53792, 2.21161), Color(0.00533333, 0.0266667, 0.00533333) )); 
  
  scene->setCamera(new PinholeCamera(Point(0, -20, 0),
                                     Point(0, 0, 0),
                                     Vector(0, 0, 1),
                                     50));
  scene->setMaxRayDepth(125 );
  scene->setMinAttenuation(.01);
  
  return scene;
}

ADD_SCENE(cloister)
