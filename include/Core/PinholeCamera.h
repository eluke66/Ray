
#ifndef PinholeCamera_h
#define PinholeCamera_h

#include "Core/Camera.h"
#include "Core/Point.h"
#include "Core/Vector.h"

class PinholeCamera : public Camera {
  friend class PinholeCameraMover;
 public:
  PinholeCamera(const Point& eye, const Point& lookat, const Vector& up, double hfov);
  virtual ~PinholeCamera();

  virtual void preprocess(double aspect_ratio);
  virtual void makeRay(Ray& ray, const RenderContext& context, double x, double y,
		       double a, double b) const;

 protected:
  PinholeCamera(const PinholeCamera&);
  PinholeCamera& operator=(const PinholeCamera&);

  Point eye;
  Point lookat;
  Vector up;
  double hfov;

  Vector u;
  Vector v;
  Vector lookdir;
  double _aspect;
};

#include "Core/FrameFunc.h"
#include <iostream>
template <typename T>
struct Diff {
  Diff() : isSet(false), _velocity(-1), _accel(0.0) {}
  void set( const T &p )
  {
    pt = p; isSet = true;
  }
  void setVelocity( double velocity, double accel=0.0 )
  {
    _velocity = velocity;
    _accel    = accel;
  }
  double calcTime(double &velocity)
  {
    std::cerr << "CalcTime: _vel = " << _velocity << " vel = " << velocity << std::endl;
    if ( _velocity == -1 )
      _velocity = velocity;

    double dist = Vector(pt).length();
    if ( _accel == 0.0 )
      return  dist / _velocity;
    
    double a = _accel * 0.5;
    double b = _velocity;
    double c = -dist;
    
    double disc = b*b - 4 * a * c;
    if ( disc <= 0.0 ) {
      std::cerr << "Acceleration with velocity won't get us to next point!"
		<< std::endl;
      std::cerr << "V/A/D " << _velocity << " " <<
	_accel << " " << dist << std::endl;
      assert(0);
    }
    disc = sqrt(disc);
    
    double t1 = (-b + disc) / (2.0 * a);
    double t2 = (-b - disc) / (2.0 * a);

    assert( t1 >=0 || t2 >= 0 );

    double retVal;
    
    if ( t1 < 0 )
      retVal = t2;
    else if ( t2 < 0 )
      retVal = t1;
    else   
      retVal = std::min( t1, t2 );

    std::cerr << "\tTime is " << retVal << std::endl;
    velocity = _velocity + _accel * retVal;
    std::cerr << "Velocity in: " << _velocity << " out " << velocity
	      << std::endl;
    return retVal;
  }
  
  T pt;
  bool isSet; 
  double _velocity, _accel;
};
#include <vector>

struct EAU {
  void setEye( const Point &eye, double velocity=-1.0, double accel=0.0 )
  {
    _eye.set(eye);
    _eye.setVelocity(velocity,accel);
  }
  Diff<Point> _eye;
  Diff<Point> _at;
  Diff<Vector> _up;
};
  
class PinholeCameraMover : public FrameFunc
{
public:
  PinholeCameraMover() : _camera(NULL) {}
  virtual ~PinholeCameraMover() {}
  
  virtual void doRegister( Scene *scene );

  void addPoint( const EAU &eau ) {
    _points.push_back( eau );
  }
  void addPoint( const Point &eye, bool eyeSet,
		 const Point &at, bool atSet,
		 const Vector &up, bool upSet )
  {
    EAU eau;
    if ( eyeSet ) eau._eye.set(eye);
    if ( atSet  ) eau._at.set(at);
    if ( upSet  ) eau._up.set(up);
    _points.push_back( eau );
  }

  virtual void setNumFrames( int nFrames );
  virtual void operator()(int whichFrame);

protected:
  PinholeCamera *_camera;
  unsigned _currentPoint;
  
  std::vector<EAU> _points;
  std::vector<int> _frames; // Frames per point
};
#endif

