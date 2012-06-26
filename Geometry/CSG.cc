
#include "Geometry/CSG.h"
#include "Core/BoundingBox.h"
#include "Core/HitRecord.h"
#include "Core/Math.h"
#include "Core/Point.h"
#include "Core/Ray.h"
#include "Core/Vector.h"
#include "Core/Matrix.h"
#include "Geometry/Group.h"
#include "Geometry/Plane.h"
#include <iostream>
#include <sstream>
#include <string.h>
using namespace std;

CSG::CSG(Material* material)
  : Primitive(material), _csg(NULL), _group(NULL)
{
  ALLOC;
  
}
CSG::CSG(CSG_Item* item)
  : Primitive(NULL), _csg(NULL), _group(NULL)
{
  if ( item )
    setCSG(item);
}

CSG::~CSG()
{
}

void CSG::transform(const Matrix4 &matrix) {
  assert(_group);
  _group->transform(matrix);
}

void CSG::getBounds(BoundingBox& bbox) const
{
  assert(_group);
  _group->getBounds(bbox);
}

void CSG::computeUVW(Vector& uvw, const RenderContext& context,
		     const Ray& ray, const HitRecord& hit) const
{
  assert( _group );
  // NOT DONE  _group->computeUVW( uvw, context, ray, hit );
}

void CSG::getNormal(Vector& normal, RenderContext&c, const Point& hitpos,
                    const Ray& ray, const HitRecord& hit) const
{
  hit.getPrimitive()->normal( normal, c, hitpos, ray, hit );
}


void CSG::intersect(HitRecord& hit,
		    const RenderContext& context,
		    const Ray& ray) const
{
  assert(_group);
  double totalT = 0.0;
  
  char scratchpad[128];
  memset( scratchpad, 0, 128 );
  std::ostringstream isectRec;
  
  Ray ray1(ray);

  const Material *lastMat = NULL;
  while (1) {
    HitRecord hit1(hit);
    _group->intersect(hit1, context, ray1);
    if ( hit1.getPrimitive() == NULL ) {
      return;
    }
    totalT += hit1.minT();
    
    PrimMap::const_iterator ci = _map.find( (Primitive *)hit1.getPrimitive() );
    if ( ci == _map.end() ) {
#if 0
      std::cerr << "Couldn't find item for "
		<< (Primitive *)hit1.getPrimitive()
		<< " " << typeid(*hit1.getPrimitive()).name()
		<< std::endl;
#endif
      return;
    }
    assert( ci != _map.end() );
    int idx = ci->second;
    assert( idx >= 0 );
    // If turning this bit off makes it work, then we have just exited
    // a negative. Use the previous material.
    bool hitNegative = scratchpad[ idx ];
    Plane * pl = dynamic_cast<Plane *>( (Primitive *)hit1.getPrimitive() );
    
    if ( !pl || ( Dot( pl->normal(), ray1.direction() ) < 0.0 ) ) {
      scratchpad[ idx ] = !scratchpad[idx];
    }

    // If we haven't hit a NOT part, save this material.
    if ( !hitNegative && hit1.getMaterial() ) {
      lastMat = hit1.getMaterial();
      isectRec << " Set last to mat " << lastMat << " ";
    }

    if ( _csg->evaluate(scratchpad) ) {
      const Material *mat = this->matl;
      if ( !mat )
	mat = hit1.getMaterial();

      // We always use the current material unless it's a negative
      // hit. Then, we use the last material (which will be the last
      // non-NOT item we hit).
      if ( !mat ) {
	isectRec << " set mat away from " << this->matl << " ";
      }
      
      if ( (hitNegative || !mat) && lastMat )
	mat = lastMat;

      // Should we assert mat here?
      if ( !hitNegative && !mat ) {
	std::cerr << "No mat, crabby!" << std::endl;
	std::cerr << isectRec.str() << std::endl;
	assert(mat);
      }
      
      if ( !hitNegative ) {
	hit.hit( totalT, hit1.getPrimitive(), mat );
	return;
      }
    }
    Point origin = ray1.origin() + hit1.minT() * ray1.direction();
    ray1 = Ray( origin, ray1.direction() );
  }

}


void CSG::setCSG( CSG_Item *item )
{
  _csg = item;
  assert( _csg );

  CSGPrimitiveList l;
  _csg->getPrimitiveList(l);
  _group = new Group;
  for ( unsigned i = 0; i < l.size(); ++i ) {
    l[i]->setIdx(i);
    Primitive *prim = l[i]->getPrimitive();
    _group->addObject(prim);
    _map[prim] = i;
  }
}

CSG_Item *CSG_Item::MakeCSG(Group *group)
{
  assert(group);
  std::vector<Object*>& objs = group->getObjects();
  CSG_Item *top = NULL;
  
  for (unsigned i = 0; i < objs.size(); ++i ) {
    Primitive * prim = dynamic_cast<Primitive *>(objs[i]);
    
    // Primitive
    if ( prim ) {
      CSG_Primitive *p = new CSG_Primitive(prim);
      if ( ! top )
	top = p;
      else
	top = new CSG_Or( top, p );
    }
      
    // Another group
    else {
      Group * grp = dynamic_cast<Group *>(objs[i]);
      if (! grp )
	assert( !"NOT A GROUP OR A PRIMITIVE!" );

      CSG_Item *item = CSG_Item::MakeCSG(grp);
      if ( !top )
	top = item;
      else
	top = new CSG_Or(top, item);
    }
       

  }

  return top;
}
