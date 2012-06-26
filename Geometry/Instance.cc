
#include "Geometry/Instance.h"
#include "Geometry/Primitive.h"
#include "Geometry/Group.h"
#include "Core/HitRecord.h"
#include "Core/BoundingBox.h"
#include "Material/Material.h"
#include "Core/Ray.h"
#include "Core/Vector.h"

Instance::Instance(const Matrix4 &mat, Object *object, Material* matl )
  : Primitive(matl), _mat(mat), _object(object), _matl(matl), _bboxed(false)
{
  ALLOC;
  _mInverse = mat.invert();
  assert( _object );
}

Instance::~Instance()
{
}

void Instance::preprocess()
{
  Point p[8];
  
  BoundingBox b;
  _object->getBounds(b);
  
  p[1] = Point(b.getMin().x(), b.getMin().y(), b.getMin().z());
  p[2] = Point(b.getMax().x(), b.getMin().y(), b.getMin().z());
  p[3] = Point(b.getMax().x(), b.getMax().y(), b.getMin().z());
  p[4] = Point(b.getMin().x(), b.getMax().y(), b.getMin().z());
  p[5] = Point(b.getMin().x(), b.getMin().y(), b.getMax().z());
  p[6] = Point(b.getMax().x(), b.getMin().y(), b.getMax().z());
  p[7] = Point(b.getMax().x(), b.getMax().y(), b.getMax().z());
  p[0] = Point(b.getMin().x(), b.getMax().y(), b.getMax().z());

  for ( int i = 0; i < 8; ++i )
    p[i] = _mat * p[i];

  for ( int i = 0; i < 8; ++i )
    _bbox.extend( p[i] );

  _bboxed = true;
}

Ray
Instance::transformRay( const Ray &ray ) const {
  Point pTrans = _mInverse * ray.origin();
  Vector vTrans = _mInverse.transform(ray.direction());
  vTrans.normalize();
  return Ray( pTrans, vTrans );
}

void
Instance::intersect(HitRecord& hit, const RenderContext& context,
		    const Ray& ray) const {

  // Transform ray.
  Ray tRay = transformRay( ray );

  double minT = hit.minT();
  
  // Intersect ray with real object
  _object->intersect( hit, context, tRay );

  bool hitThis = (hit.minT() < minT);
  
  if ( hitThis ) {
    hit.realPrim = hit.getPrimitive();
    hit.setPrimitive( this );
    
    if ( _matl )
      hit.setMaterial( _matl );
  }
}

void 
Instance::getNormal(Vector& normal, RenderContext& context,
		    const Point& hitpos, const Ray& ray,
		    const HitRecord& hit) const {

  // Hitpos is in transformed space. Get the position in regular
  // space. Then, get normal in regular space, and transform the
  // normal back to transformed space. Normalize the normal vector as
  // well.
  Point hitRegular = _mInverse * hitpos;
  Vector normalRegular;

  const Primitive *prm = hit.realPrim;
  assert(prm);

  // Sanity check - make sure that if we're an instanced primitive the
  // scratchpad actually contains the primitive.
  const Primitive *objPrim = dynamic_cast<const Primitive *>(_object);
  if ( objPrim ) {
    if ( prm != objPrim ) {
      const Instance *inst = dynamic_cast<const Instance *>(prm);
      if ( inst ) {
	assert(_object == inst->instancedObject());
	prm = objPrim;
      }
      else {
	assert(0);
      }
      
    }
  }

  prm->normal( normalRegular, context, hitRegular, ray, hit );
  
  normal = _mat.transform(normalRegular);
  normal.normalize();
}

void Instance::computeUVW(Vector& uvw, const RenderContext& context,
                           const Ray& ray, const HitRecord& hit) const
{
  Point hitpos = ray.origin()+ray.direction()*hit.minT();
  uvw = Vector(hitpos);
}

void Instance::getBounds(BoundingBox& bbox) const {
  assert( _bboxed );
  bbox.extend(_bbox.getMin());
  bbox.extend(_bbox.getMax());
}
