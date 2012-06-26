
#ifndef Building_h
#define Building_h

#include "Geometry/Primitive.h"
#include "Core/Point.h"
class Ray;
class Material;
class Group;

class Building;

class BuildingFactory {
public:
  // In the future, also add a new type of building (little ones, in a
  // square).
  typedef enum { SKYSCRAPER,
		 MIDSIZE,
		 BRICK_BUILDING,
		 NUM_BUILDING_TYPES } BuildingType;

  // In the future, also set:
  // max building height
  // Num building level items.
  // Num cutaways
  // Cutaway size in x, y
  // % chance to get a cutaway.
  static void Initialize( float unitsPerFoot, float maxHeight );
  static BuildingType RandomBuilding();
  static Building * CreateBuilding( BuildingType type,
				    const Point &min,
				    const Point &max,
				    float distanceFactor );

  static Material *Concrete(double variance=1.0);
  
private:
  BuildingFactory();
  ~BuildingFactory();

  static double GetBuildingHeight( BuildingType type,
				   const Point &min,
				   const Point &max );
  static void CreateMaterials( BuildingType type,
			       double hSize,
			       double vSize,
			       Material *&win,
			       Material *&nonWin,
			       Material *&roof);
  static void AddBuildingDetail( BuildingType type,
				 float distanceFactor,
				 const Point &min,
				 const Point &max,
				 Point &roofMin,
				 Point &roofMax,
				 Building *building,
				 Material *windows,
				 Material *nonWin,
				 Material *roof);
  static void AddRooftop( BuildingType type,
			  float distanceFactor,
			  double height,
			  const Point &roofMin,
			  const Point &roofMax,
			  Building *building,
			  Material *windows,
			  Material *nonWin );

  static void GetNonDecoratedSize( Point &ndMin,
				   Point &ndMax,
				   BuildingType type,
				   const Point &min,
				   const Point &max );
  static float _UnitsPerFoot;
  static float _MaxHeight;
};


class Building : public Primitive {
public:
  Building(Material* sideMatl,
	   Material* topMatl,
	   Material* roof,
	   const Point& p1, const Point& p2);
  virtual ~Building();

  virtual void transform(const Matrix4 &matrix);
  virtual void getBounds(BoundingBox& bbox) const;
  virtual void intersect(HitRecord& hit, const RenderContext& context, const Ray& ray) const;
  virtual void getNormal(Vector& normal, RenderContext& context,
			 const Point& hitpos, const Ray& ray, const HitRecord& hit) const;
  virtual void computeUVW(Vector& uvw, const RenderContext& context,
			  const Ray& ray, const HitRecord& hit) const;
  void add(Object *obj);
 protected:
  Material *_sideMatl;
  Material *_topMatl;
  Material *_roof;
  Group    *_parts;
};

#endif
