
#ifndef Camera_h
#define Camera_h

class Ray;
class RenderContext;

class Camera {
 public:
  Camera();
  virtual ~Camera();

  virtual void preprocess(double aspect_ratio) = 0;
  virtual void makeRay(Ray& ray, const RenderContext& context, double x, double y,
		       double a, double b) const = 0;
  virtual int numSamples() const { return 1; }
 private:
  Camera(const Camera&);
  Camera& operator=(const Camera&);
};

#endif

