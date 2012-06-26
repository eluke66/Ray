
#ifndef GriddedGroup_h
#define GriddedGroup_h

#include "Geometry/Group.h"
#include "Core/Array3.h"
#include "Core/Vector.h"

class GriddedGroup : public Group {
 public:
  GriddedGroup(double cellfactor);
  virtual ~GriddedGroup();

  virtual void preprocess();
  virtual void intersect(HitRecord& hit, const RenderContext& context, const Ray& ray) const;
 private:
  GriddedGroup(const GriddedGroup&);
  GriddedGroup& operator=(const GriddedGroup&);

  double cellfactor;
  Point min, max;
  Vector diag;
  Vector cellsize;
  Vector inv_cellsize;
  Array3<int>  cells;
  const Object** lists;

  void transformToLattice(const BoundingBox& box,
                          int& sx, int& sy, int& sz, int& ex, int& ey, int& ez) const;
};

#endif
