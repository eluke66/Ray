
#ifndef Group_h
#define Group_h

#include "Geometry/Object.h"
#include "Core/BoundingBox.h"
#include <vector>

class Group : public Object {
 public:
  Group();
  virtual ~Group();

  virtual void setMaterial(Material *mat, bool override);
  void addObject(Object* object);
  virtual void transform(const Matrix4 &matrix);
  virtual void preprocess();
  virtual void getBounds(BoundingBox& bbox) const;
  virtual void intersect(HitRecord& hit, const RenderContext& context, const Ray& ray) const;

  std::vector<Object*>& getObjects() { return objects; }
 protected:
  std::vector<Object*> objects;
  BoundingBox _bbox;
  bool _haveBounds;
  
 private:
  Group(const Group&);
  Group& operator=(const Group&);
};

#endif

