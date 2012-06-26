
#ifndef ThinLensCamera_h
#define ThinLensCamera_h

#include "Core/PinholeCamera.h"
#include "Core/Point.h"
#include "Core/Vector.h"

class ThinLensCamera : public PinholeCamera {
 public:
  ThinLensCamera(const Point& eye, const Point& lookat, const Vector& up, double hfov, double distance, double aperture, int numSamples);
  virtual ~ThinLensCamera();
  
  virtual void preprocess(double aspect_ratio);
  virtual void makeRay(Ray& ray, const RenderContext& context, double x, double y,
		       double a, double b) const;

  virtual int numSamples() const { return _numSamples; }
  
 private:
  ThinLensCamera(const ThinLensCamera&);
  ThinLensCamera& operator=(const ThinLensCamera&);
  double _distance;
  double _lensRadius;
  int    _numSamples;
};

#endif

