#include "all.h"
#include "Core/PerlinNoise.h"
#include "Geometry/Mountains.h"

class sinfunction {
 public:
  double operator()(double x, double y)
  {
    double r2 = (x*x+y*y)*4;
    return (sin(r2)/r2+1)*0.5;
  }
};

static double _z = 256 * drand48();
static double dmin = 1000;
static double dmax = -1000;
static double dtot = 0.0;
static unsigned calls = 0;

struct goo {
  goo() : _rpt(false) {}
  void report() { _rpt = true; }
  ~goo() {
    if (_rpt) {
      std::cerr << "MTMIN: " << dmin << " MTMAX: " << dmax << std::endl;
      std::cerr << "AVG: " << dtot / calls << std::endl;
    }
  }
  bool _rpt;
};

static goo goof;

double XF = 80.0;
double YF = 20.0;

class noisefunction1 {
public:
  double operator()(double x, double y)
  {
    //double d = turbulenceAbs( 10, Point(x/13.0,y/13.0,0.5), 2.0, 0.5 );
    double xx = x/(1.3*XF);
    //    std::cerr << "xx is " << xx << std::endl;
    double d = turbulenceAbs( 8, Point(xx,y/(1.3*YF),0.5), 2.0, 0.5 );
    
    if ( d < dmin )
      dmin = d;
    if ( d > dmax )
      dmax = d;
    dtot += d;
    calls++;
    if ( d <= 0.0 )
      d = 0.0;
    //    if ( d >= 1.0 )
    //  d = 1.0;

    // Exponential falloff in the Y direction
    // 180 -> 340 )
    
    /*
      p = e^(kt)
      p = 0.01 when t = 160
      0.01 = e^(160k)
      ln(0.01) = 160k
      k = ln(0.01) / 160 = 0.029
    */
    double prod = exp(-0.029 * (y-180));
    if ( prod > 1.0 )
      prod = 1.0;
    d *= prod;
    return d;
  }
};
  
Scene* scenemtn()
{
  goof.report();
  Scene* scene = new Scene();
  scene->setBackground(new ConstantBackground(Color(.1, .2, .3)));
  Group* world = new Group();
  Material * g = new LambertianMaterial( Color(0.5, 0.5, 0.5), 0.6, 0.4 );
  Material *g1 =
    new CheckerMaterial( Point(0,0,0),
			 Vector(0.5,0.5,0),
			 Vector(-0.5,0.5,0),
			 new LambertianMaterial( Color(1,1,1), 0.6, 0.4),
			 new LambertianMaterial( Color(0,0,0), 0.6, 0.4) );
				     
  world->addObject( new Plane( g, Vector(0,0,1), Point(0,0,0) ) );
  
  Material* sinmatl = new LambertianMaterial(Color(1, 1, 0), .8, .2);
  int xres = 32;
  double fov = 55;
  double epsilon = 0.25;
  double e = epsilon*tan(fov*M_PI/180/2)/xres;
#if 0
  world->addObject(new QAEB<noisefunction1>(sinmatl, Point(-4*XF,0*YF+180,-7),
                                         Point(4*XF,8*YF+180,60), e));
#else
  world->addObject( new Mountains( sinmatl,
				   Point(-4*XF,0*YF+180,-7),
				   Point(4*XF,8*YF+180,60) ) );
#endif
  scene->setObject(world);
  scene->setMaxRayDepth(10);
  scene->setAmbient(Color(.4, .4, .4));
  //  scene->addLight(new PointLight(Point(0, 130, 10), Color(.9,.9,.9)));
  scene->addLight(new PointLight(Point(20, -30, 30), Color(100.9)));
  scene->setCamera(new PinholeCamera(Point(0, -60, 22.5),
				     Point(0, 0, 0),
				     Vector(0, 0, 1),
				     fov));

  return scene;
}

ADD_SCENE(scenemtn)
  
