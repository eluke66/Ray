
#ifndef Bridge_h
#define Bridge_h

#include "Geometry/Primitive.h"
#include "Core/Point.h"
class Ray;
class Material;
class Group;



class Bridge : public Primitive {
public:
  Bridge(Material *matl, const Point& minPoint, const Point& maxPoint,
	 double height, double landOrWater);
  virtual ~Bridge();

  virtual void transform(const Matrix4 &matrix);
  virtual void getBounds(BoundingBox& bbox) const;
  virtual void preprocess();
  virtual void intersect(HitRecord& hit, const RenderContext& context, const Ray& ray) const;
  virtual void getNormal(Vector& normal, RenderContext& context,
			 const Point& hitpos, const Ray& ray, const HitRecord& hit) const;
  virtual void computeUVW(Vector& uvw, const RenderContext& context,
			  const Ray& ray, const HitRecord& hit) const;

  void setSupportMatl(Material *matl) { _supportMatl = matl; }
  void setTowerMatl(Material *matl) { _towerMatl = matl; }
  
protected:
  Group  *_parts;
  Point   _minPoint, _maxPoint;
  double  _height;
  double  _landOrWater;
  Material *_supportMatl;
  Material *_towerMatl;

  short idx, oidx;

  virtual void makeRoadbed(double railThickness);
  Group * createRoadbed(double railThickness,
			const Point &minPoint,
			const Point &maxPoint);
};

class PlatformBridge : public Bridge {
public:
  PlatformBridge(Material *matl, const Point& minPoint, const Point& maxPoint,
		 double height, double landOrWater);
  virtual ~PlatformBridge();

  virtual void preprocess();
};

class SuspensionBridge : public Bridge {
public:
  SuspensionBridge(Material *matl, const Point& minPoint, const Point& maxPoint,
		   double height, double landOrWater, double spacingBetweenCables, int numTowers=2);
  virtual ~SuspensionBridge();

  virtual void preprocess();

  void setMainCableThickness(double thickness) { _mainCableThickness = thickness; }
  void setVerticalCableThickness(double thickness) { _verticalCableThickness = thickness; }
  void setTowerThickness( double thickness ) { _towerThickness = thickness; }
  
  typedef enum {ARCH_TOP, SQUARE_ONE, SQUARE_TWO, SQUARE_TOP, NONE} TowerType;
 protected:
  double  _cableSpacing;

  double _a;
  double _midX;
  double _midZ;
  double _mainCableThickness;
  double _verticalCableThickness;
  double _towerThickness;
  int    _numTowers;
  TowerType _towerType;
  
  double chainHeight( double x );
  Group * makeTower( const Point2D &minPoint, const Point2D &maxPoint,
		     double midHeight, double top, double bottom );
  void createCable( const Point &p1, const Point &p2, Material * matl, Group *group );
  Group * makeIntertowerCables( const Point &startPoint, const Point &endPoint, double bottom, Material *matl );
  Group * makeInitialAnchorCables(  const Point &startPoint, const Point &endPoint, double bottom, Material *matl, const Point2D &initial );
  Group * makeFinalAnchorCables(  const Point &startPoint, const Point &endPoint, double bottom, Material *matl, const Point2D &final );
  Group * makeCables( const Point &startPoint, const Point &endPoint, double bottom, Material *matl,
		      const Point2D &initial, const Point2D &final );
  void addVerticalCables( double startX, double endX, double bottom, double Y, Group *group );
};


class ArchBridge : public Bridge {
public:
  ArchBridge(Material *matl, const Point& minPoint, const Point& maxPoint,
	     double height, double landOrWater);
  virtual ~ArchBridge();

  virtual void preprocess();
  
private:
  double _a, _b;
  void createArch( const Point &p1, const Point &p2, Material * matl, Group *group );
  double getArchHeight( double spot );
};
  
#endif
