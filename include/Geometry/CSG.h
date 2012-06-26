
#ifndef CSG_h
#define CSG_h

#include "Geometry/Primitive.h"
#include "Core/Point.h"
class Ray;

struct CSG_Item;
class Group;
#include <map>


class CSG : public Primitive {
 public:
  CSG(CSG_Item* item);
  CSG(Material* material);
  virtual ~CSG();

  void setCSG( CSG_Item *item );
  
  virtual void transform(const Matrix4 &matrix);
  virtual void getBounds(BoundingBox& bbox) const;
  virtual void intersect(HitRecord& hit, const RenderContext& context, const Ray& ray) const;
  virtual void getNormal(Vector& normal, RenderContext& context,
			 const Point& hitpos, const Ray& ray, const HitRecord& hit) const;
  virtual void computeUVW(Vector& uvw, const RenderContext& context,
			  const Ray& ray, const HitRecord& hit) const;
 private:
  CSG_Item *_csg;
  Group *  _group;
  typedef std::map<Primitive *,int> PrimMap;
  PrimMap  _map;
};

#include <vector>
struct CSG_Primitive;
typedef std::vector<CSG_Primitive *> CSGPrimitiveList;

struct CSG_Item {
  friend class CSG;
  virtual ~CSG_Item() {}
  virtual bool evaluate(const char * hit) = 0;
  
  virtual void getPrimitiveList(CSGPrimitiveList &l) = 0;

  static CSG_Item *MakeCSG(Group *group);
};

struct CSG_Primitive : public CSG_Item {
  CSG_Primitive( Primitive *prim) : _prim(prim) {}
  Primitive * getPrimitive() { return _prim; }
  
  virtual bool evaluate(const char * hit) { return hit[_idx] > 0; }
  virtual void getPrimitiveList(CSGPrimitiveList &l) { l.push_back(this); }

  void setIdx(int idx) { _idx = idx; }

protected:
  int _idx;
  Primitive *_prim;
};

struct CSG_And : public CSG_Item {
  CSG_And( CSG_Item *op1, CSG_Item *op2 ): _op1(op1), _op2(op2) {}
  virtual bool evaluate(const char * hit) {
    return _op1->evaluate(hit) && _op2->evaluate(hit);
  }
  virtual void getPrimitiveList(CSGPrimitiveList &l) {
    _op1->getPrimitiveList(l);
    _op2->getPrimitiveList(l);
  }
protected:
  CSG_Item *_op1, *_op2;
};
struct CSG_Or : public CSG_Item {
  CSG_Or( CSG_Item *op1, CSG_Item *op2 ): _op1(op1), _op2(op2) {}
  virtual bool evaluate(const char * hit) {
    return _op1->evaluate(hit) || _op2->evaluate(hit);
  }
  virtual void getPrimitiveList(CSGPrimitiveList &l) {
    _op1->getPrimitiveList(l);
    _op2->getPrimitiveList(l);
  }
protected:
  CSG_Item *_op1, *_op2;
};
struct CSG_Not : public CSG_Item {
  CSG_Not(CSG_Item *op) : _op(op) {}
  virtual bool evaluate(const char * hit) {
    return !_op->evaluate(hit);
  }
  virtual void getPrimitiveList(CSGPrimitiveList &l) {
    _op->getPrimitiveList(l);
  }
protected:
  CSG_Item *_op;
};

#endif
