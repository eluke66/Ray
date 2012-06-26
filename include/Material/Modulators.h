#include <vector>
#include "Core/Point.h"

class RenderContext;
class Ray;
class HitRecord;
class Image;
class Object;

class Modulator {
public:
  virtual ~Modulator() {}
  virtual void getWeights( std::vector<double> &weights,
			   RenderContext& context,
			   const Ray& ray,
			   const HitRecord& hit ) const = 0;
};


class CheckerModulator : public Modulator {
public:
  CheckerModulator(const Point& origin, const Vector& v1, const Vector& v2);
  virtual ~CheckerModulator() {}
  void getWeights( std::vector<double> &weights,
		   RenderContext& context,
		   const Ray& ray,
		   const HitRecord& hit ) const;
private:
  Point _origin;
  Vector _v1, _v2, _v3;
};

class GradientModulator : public Modulator {
public:
  GradientModulator(bool horizontal=true, float repeat=1.0) :
    _hRepeat(repeat), _vRepeat(1.0)
  {
    if ( !horizontal ) {
      // Swap to vertical gradient
      _vRepeat = repeat;
      _hRepeat = 1.0;
    }
  }
  GradientModulator(float hRepeat, float vRepeat ) :
    _hRepeat(hRepeat), _vRepeat(vRepeat) {}
  virtual ~GradientModulator() {}
  void getWeights( std::vector<double> &weights,
		   RenderContext& context,
		   const Ray& ray,
		   const HitRecord& hit ) const;
private:
  float _hRepeat, _vRepeat;
};

class TextureModulator : public Modulator {
public:
  TextureModulator(Image *image,
		   double uRepeat = 1.0, double vRepeat = 1.0f) :
    _image(image), _uRepeat(uRepeat), _vRepeat(vRepeat) {}
  virtual ~TextureModulator() {}
  void getWeights( std::vector<double> &weights,
		   RenderContext& context,
		   const Ray& ray,
		   const HitRecord& hit ) const;
private:
  Image *_image;
  float _uRepeat, _vRepeat;
};

class SimpleBuildingModulator : public Modulator {
public:
  // HGap and VGap are the % of space on each side of the window that
  // is not window.
  // hRepeat and vRepeat are the number of windows in each dimension.
  SimpleBuildingModulator( double hGap, double vGap,
			   double hRepeat, double vRepeat);
  virtual void getWeights( std::vector<double> &weights,
			   RenderContext &context,
			   const Ray &ray,
			   const HitRecord &hit ) const;

private:
  double _hGap, _vGap;
  double _hRepeat, _vRepeat;
};


#include "Core/BoundingBox.h"

/*
  Input material order:
  - normal
  - hroad,
  - vroad,
  - intersection
  - sidewalk
*/
class CityGridModulator : public Modulator {
public:

  // Min and max are city bounds in real coordinates.
  CityGridModulator( const Point2D &min, const Point2D &max, int N=100 ) :
    _min(min), _max(max), _N(N)
  {
    _grid = new std::vector<unsigned> *[ N ];
    for ( int i = 0; i < N; ++i ) {
      _grid[i] = new std::vector<unsigned>[ N ];
    }
  }
  
  // Default is no building. Other options are hroad,
  // vroad, intersection, sidewalk.
  typedef enum { HROAD=1, VROAD, INTERSECTION, SIDEWALK } CityMat;
  void addMaterial( CityMat material,
		    const Point2D &min, const Point2D &max );
  
  virtual void getWeights( std::vector<double> &weights,
			   RenderContext &context,
			   const Ray &ray,
			   const HitRecord &hit ) const;

private:
  const Point2D _min, _max;
  int _N;

  // Each addMaterial adds the appropriate min/max to the global list,
  // and the index to each grid section
  struct bbmat {
    bbmat(const BoundingBox2D &box, CityMat mat ) : box(box), mat(mat) {}
    BoundingBox2D box;
    CityMat mat;
  };
  std::vector<bbmat> _matList;
  std::vector<unsigned> **_grid;
};

// Splits object between two different materials.
class SplitModulator : public Modulator {
public:
  typedef enum { XSPLIT, YSPLIT, ZSPLIT } SplitType;
  SplitModulator( double boundary, Object *obj, SplitType split=XSPLIT, double repeat=1.0 ):
    _boundary(boundary), _obj(obj), _split(split), _repeat(repeat)
  {
  }

  virtual void getWeights( std::vector<double> &weights,
			   RenderContext &context,
			   const Ray &ray,
			   const HitRecord &hit ) const;
private:
  double _boundary;
  Object * _obj;
  SplitType _split;
  double _repeat;
};
