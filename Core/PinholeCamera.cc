
#include "Core/PinholeCamera.h"
#include "Core/Ray.h"
#include <math.h>

PinholeCamera::PinholeCamera(const Point& eye, const Point& lookat, const Vector& up,
                             double hfov)
  : eye(eye), lookat(lookat), up(up), hfov(hfov)
{
}

PinholeCamera::~PinholeCamera()
{
}

void PinholeCamera::preprocess(double aspect_ratio)
{
  _aspect = aspect_ratio;
  lookdir = lookat-eye;
  lookdir.normalize();
  u = Cross(lookdir, up);
  v = Cross(u, lookdir);
  double ulen = tan(hfov/2.*M_PI/180.);
  u.normalize();
  u *= ulen;
  double vlen = ulen/aspect_ratio;
  v.normalize(); 
  v *= vlen;
}

void PinholeCamera::makeRay(Ray& ray, const RenderContext& context, double x, double y,
			    double, double) const
{
  Vector direction = lookdir+u*x+v*y;
  direction.normalize();
  ray = Ray(eye, direction);
}

#include <iostream>
#ifdef OLD_WAY
void PinholeCameraMover::setNumFrames( int nFrames )
{
  FrameFunc::setNumFrames( nFrames );

  std::vector<double> distances;
  double totalDistance = 0.0;
  
  // For each item, figure out the distance.
  for ( unsigned point = 0;
	point < _points.size();
	++point )
    {
      // Just eye for now...
      distances.push_back( Vector(_points[ point ]._eye.pt).length() );
      totalDistance += distances[ point ];
    }
  //  std::cerr << "Total: " << totalDistance << std::endl;

  int totalFrames = 0;
  double currentDistance = 0;
  for ( unsigned point = 0;
	point < _points.size();
	++point )
    {
      // Can't just round - will lose frames! Need to see how many
      // frames should be allocated by this time, and add ensure that
      // the total frames reaches that point.
      currentDistance += distances[point];
      int numFramesAllocAtThisPoint = int(nFrames * (currentDistance / totalDistance));
      int numFrames = numFramesAllocAtThisPoint - totalFrames;
      totalFrames += numFrames;

      //std::cerr << "\tPoint " << point << " " << " dist fraction is " << (distances[ point ] / totalDistance) << " " 
      //		<< numFrames << " current total is " << totalFrames << std::endl;
      _frames.push_back( totalFrames );
    }
  
  _currentPoint = 1;

  std::vector< EAU > newPts;
  EAU start;
  start._eye.set( _camera->eye );
  start._at.set ( _camera->lookat );
  start._up.set ( _camera->up );
  
  newPts.push_back( start );
  for ( unsigned p = 0;
	p < _points.size();
	++p )
    {
      start._eye.set( newPts[p]._eye.pt + _points[p]._eye.pt );
      start._at.set( newPts[p]._at.pt + _points[p]._at.pt );
      start._up.set( newPts[p]._up.pt + _points[p]._up.pt );
      newPts.push_back(start);
    }

  _points = newPts;
}

void PinholeCameraMover::operator()(int whichFrame)
{
  assert( _camera );
  // If we're on the final point, just plug that in.
  if ( _currentPoint == _points.size() - 1 )
    {
      _camera->eye    = _points[ _currentPoint ]._eye.pt;
      _camera->lookat = _points[ _currentPoint ]._at.pt;
      _camera->up     = _points[ _currentPoint ]._up.pt;
    }
  else
    {
      //      std::cerr << " ============== FRAME " << whichFrame << " ==================" << std::endl;
      //std::cerr << "CurrentPoint: " << _currentPoint
      //		<< " of min/max " << _frames[ _currentPoint-1 ] << "/" << _frames[ _currentPoint ] << std::endl;
      int prevFrameCount = _frames[ _currentPoint-1 ];
      assert( whichFrame >= prevFrameCount );
      assert( whichFrame <= _frames[ _currentPoint ] );
      double x = double( whichFrame - prevFrameCount ) / double( _frames[ _currentPoint ] - prevFrameCount );
      //std::cerr << "X IS " << x << std::endl;
      double factor = ( 1.0 - cos( M_PI * x ) ) * 0.5;
      //std::cerr << "Factor is " << factor << std::endl;
      Point prev = _points[ _currentPoint-1 ]._eye.pt;
      Point next = _points[ _currentPoint   ]._eye.pt;
      //std::cerr << "Prev eye is " << prev << std::endl;
      //std::cerr << "Next eye is " << next << std::endl;
      //std::cerr << " Camera is currently: " << _camera->eye << std::endl;
      _camera->eye = Point(prev + (next-prev) * factor);
      //std::cerr << " Camera is now      : " << _camera->eye << std::endl;
      
      prev = _points[ _currentPoint-1 ]._at.pt;
      next = _points[ _currentPoint   ]._at.pt;
      _camera->lookat = Point(prev + (next-prev) * factor);

      prev = Point(_points[ _currentPoint-1 ]._up.pt);
      next = Point(_points[ _currentPoint   ]._up.pt);
      _camera->up = Vector(prev + (next-prev) * factor);

      // Move to next interval if needed
      if (whichFrame == _frames[_currentPoint]) {
	//std::cerr << "Moving to next point on frame " << whichFrame << std::endl;
	++_currentPoint;
      }
    }
  
  _camera->preprocess( _camera->_aspect );
}

#else
void PinholeCameraMover::setNumFrames( int nFrames )
{
  FrameFunc::setNumFrames( nFrames );

  std::vector<double> times;
  double totalTime = 0.0;
  double currentVelocity = 1.0;
  // For each item, figure out the time
  for ( unsigned point = 0;
	point < _points.size();
	++point )
    {
      // Just eye for now...
      double theTime = _points[ point ]._eye.calcTime( currentVelocity );
      times.push_back( theTime );
      totalTime += theTime;
    }
  std::cerr << "Total: " << totalTime << std::endl;

  _frames.push_back(0);
  
  int totalFrames = 0;
  double currentTime = 0;
  for ( unsigned point = 0;
	point < _points.size();
	++point )
    {
      // Can't just round - will lose frames! Need to see how many
      // frames should be allocated by this time, and add ensure that
      // the total frames reaches that point.
      currentTime += times[point];
      int numFramesAllocAtThisPoint = int(nFrames * (currentTime / totalTime));
      int numFrames = numFramesAllocAtThisPoint - totalFrames;
      totalFrames += numFrames;

      std::cerr << "\tPoint " << point << " " << " time fraction is " << (times[ point ] / totalTime) << " " 
      		<< numFrames << " current total is " << totalFrames << std::endl;
      _frames.push_back( totalFrames );
    }
  
  _currentPoint = 1;

  std::vector< EAU > newPts;
  EAU start;
  start._eye.set( _camera->eye );
  start._at.set ( _camera->lookat );
  start._up.set ( _camera->up );
  
  newPts.push_back( start );
  for ( unsigned p = 0;
	p < _points.size();
	++p )
    {
      start._eye.set( newPts[p]._eye.pt + _points[p]._eye.pt );
      start._at.set( newPts[p]._at.pt + _points[p]._at.pt );
      start._up.set( newPts[p]._up.pt + _points[p]._up.pt );
      newPts.push_back(start);
    }

  _points = newPts;
}

void PinholeCameraMover::operator()(int whichFrame)
{
  assert( _camera );
  std::cerr << "CURRENT: " << _currentPoint << " SIZE " << _points.size() << std::endl;
  // If we're on the final point, just plug that in.
  if ( _currentPoint == _points.size() )
    {
      _camera->eye    = _points[ _currentPoint ]._eye.pt;
      _camera->lookat = _points[ _currentPoint ]._at.pt;
      _camera->up     = _points[ _currentPoint ]._up.pt;
    }
  else
    {
      std::cerr << " ============== FRAME " << whichFrame << " ==================" << std::endl;
      std::cerr << "CurrentPoint: " << _currentPoint
      		<< " of min/max " << _frames[ _currentPoint-1 ] << "/" << _frames[ _currentPoint ] << std::endl;
      int prevFrameCount = _frames[ _currentPoint-1 ];
      assert( whichFrame >= prevFrameCount );
      assert( whichFrame <= _frames[ _currentPoint ] );
      double x = double( whichFrame - prevFrameCount ) / double( _frames[ _currentPoint ] - prevFrameCount );
      //std::cerr << "X IS " << x << std::endl;
#ifdef USE_COSINE_INTERPOLATION
      double factor = ( 1.0 - cos( M_PI * x ) ) * 0.5;
#else
      double factor = x;
#endif
      //std::cerr << "Factor is " << factor << std::endl;
      Point prev = _points[ _currentPoint-1 ]._eye.pt;
      Point next = _points[ _currentPoint   ]._eye.pt;
      //std::cerr << "Prev eye is " << prev << std::endl;
      //std::cerr << "Next eye is " << next << std::endl;
      //std::cerr << " Camera is currently: " << _camera->eye << std::endl;
      _camera->eye = Point(prev + (next-prev) * factor);
      //std::cerr << " Camera is now      : " << _camera->eye << std::endl;

#ifdef NOT_YET
      prev = _points[ _currentPoint-1 ]._at.pt;
      next = _points[ _currentPoint   ]._at.pt;
      _camera->lookat = Point(prev + (next-prev) * factor);

      prev = Point(_points[ _currentPoint-1 ]._up.pt);
      next = Point(_points[ _currentPoint   ]._up.pt);
      _camera->up = Vector(prev + (next-prev) * factor);
#endif
      
      // Move to next interval if needed
      if (whichFrame == _frames[_currentPoint]) {
	//std::cerr << "Moving to next point on frame " << whichFrame << std::endl;
	++_currentPoint;
      }
    }
  
  _camera->preprocess( _camera->_aspect );
}

#endif

#include "Core/Scene.h"
void PinholeCameraMover::doRegister( Scene *scene ) {
  _camera = dynamic_cast<PinholeCamera *>(scene->getCamera());
}

/*

Cosine interpolation:
f(x) = y1+(y2-y1)(1-Cos(pi*x))/2

y1 = prev point
y2 = next point

for us, it becomes:

Point = prev + (next - prev)*( 1-cos( M_PI * x ) )*0.5
Where x ranges between 0 and 1 (0 at prev, 1 at next).


New algorithm:
 Set num frames allocates frames.
 No need to calculate delta.
 At each frame, get previous and next point.
   X = currentFrame / totalFrames for this point.
   Calculate new point.



setNumFrames() {
  // Set num frames
  // Allocate frames per point
}

operator()() {
   ptnext = pt+1
   if ( pt == numpoints )
    ptnext = pt
    x = 1
    eye = points[pt]
    ...
    preprocess
    return
   else
    ptnext = pt+1
    
  // Get points[pt] and points[pt+1]
  // Calculate x (currentFrame / total) or
  double x = (whichFrame - frames[pt]) / (frames[pt+1] - frames[pt])
  // assert( 0 <= x <= 1 )
  eye = prev+(next-prev)*(1-cos(M_PI*x))*0.5
  ...
  Preprocess

  // Move to next interval if needed
  if (whichFrame > frames[pt])
    pt++;
}

*/
