#include "all.h"
#include "Core/PerlinNoise.h"

class sinfunction {
 public:
  void center( const Point ) {}
  double operator()(double x, double y) const
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
      std::cerr << "MIN: " << dmin << " MAX: " << dmax << std::endl;
      std::cerr << "AVG: " << dtot / calls << std::endl;
    }
  }
  bool _rpt;
};

static goo goof;

class noisefunction {
public:
  double operator()(double x, double y) const
  {
    double d = turbulenceAbs( 10, Point(x/13.0,y/13.0,0.5), 2.0, 0.5 );
    if ( d < dmin )
      dmin = d;
    if ( d > dmax )
      dmax = d;
    dtot += d;
    calls++;
    if ( d <= 0.0 )
      d = 0.0;
    if ( d >= 1.0 )
      d = 1.0;
    
    return d;
  }
  void center( const Point&) {}
};
  
Scene* scenehf()
{
  goof.report();
  Scene* scene = new Scene();
  scene->setBackground(new ConstantBackground(Color(.1, .2, .3)));
  Group* world = new Group();
  Material* sinmatl = new LambertianMaterial(Color(1, 1, 0), .8, .2);
  int xres = 16;
  double fov = 20;
  double epsilon = 0.5;
  double e = epsilon*tan(fov*M_PI/180/2)/xres;
  world->addObject(new QAEB<noisefunction>(sinmatl, Point(-4,-4,2),
                                         Point(4,4,4), e));
  scene->setObject(world);

  scene->setAmbient(Color(.4, .4, .4));
  scene->addLight(new PointLight(Point(80, 60, 100), Color(.9,.9,.9)));
  scene->addLight(new PointLight(Point(-40, -30, 50), Color(.3,.1,.1)));

  scene->setCamera(new PinholeCamera(Point(10, -18, 13.5),
                                     Point(0,0,3),
                                     Vector(0, 0, 1),
                                     fov));

  return scene;
}

ADD_SCENE(scenehf)
  
