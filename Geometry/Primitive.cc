
#include "Geometry/Primitive.h"
#include "Core/HitRecord.h"
#include "Material/Material.h"
#include "Core/Ray.h"
#include "Core/Vector.h"
#include "Geometry/Bump.h"
#include "Core/utils.h"
#include "Core/Matrix.h"

Primitive::Primitive(Material* matl, Bump *bump)
  : matl(matl), _bump(bump)
{
  //void * heapTop = sbrk(0);
  //assert( (void *)matl <= heapTop );
}

Primitive::~Primitive()
{
}

void Primitive::preprocess()
{
}


void Primitive::computeUVW(Vector& uvw, const RenderContext& context,
                           const Ray& ray, const HitRecord& hit) const
{
  Point hitpos = ray.origin()+ray.direction()*hit.minT();
  uvw = Vector(hitpos);
}

#ifdef OLD_U_V
#define ZERO(q) ABSCLOSE(normal.q(), 0.0, 1e-6)

void Primitive::normal(Vector& normal,
		       RenderContext& context,
		       const Point& hitpos,
		       const Ray& ray,
		       const HitRecord& hit) const
{
  // Normal comes in in World Space
  
  getNormal( normal, context, hitpos, ray, hit );
  if ( _bump ) {
    Vector bVector = _bump->getBump( context, ray, hit );

    Vector U,V;

    // Get an orthogonal system of U and V
    if ( !ZERO(z) ) {
      // See if X or Y is zero
      if ( ZERO(x) ) {
	if ( ZERO(y) ) 
	  U = Vector(1,0,0);
	else
	  U = Vector(1,-normal.z() / normal.y(),1);
      }
      else if ( ZERO(y) ) {
	U = Vector(-normal.x() / normal.z(),1,1);
      }
    }
    else if ( !ZERO(y) ) {
      // See if X or Z is zero
      if ( ZERO(x) ) {
	if ( ZERO(z) ) 
	  U = Vector(0,0,1);
	else
	  U = Vector(1,1,-normal.y() / normal.z());
      }
      else if ( ZERO(z) ) {
	U = Vector(-normal.x() / normal.y(),1,1);
      }
    }


    U.normalize();
    std::cerr << "U is " << U << std::endl;
    assert( ABSCLOSE(Dot( U, normal ), 0, 1e-6) );
    V = Cross( normal, U );
    std::cerr << "V is " << V << std::endl;

    // Transform the bump vector to the new coordinate system.
    // We might also want to consider using
    // Matrix3::AlignVectorWithAxis with normal for the Z axis.
    Matrix3 mat;
    mat.setRow( U, 0 );
    mat.setRow( V, 1 );
    mat.setRow( normal, 2 );

    {
      Vector bv1(bVector);
      Matrix4 mat4 = Matrix3::AlignVectorWithAxis( normal, Matrix3::Z_AXIS );
      mat4 = mat4.invert();
      bv1 = ((Matrix3)mat4) * bv1;
      std::cerr << "AVWA is " << bv1 << std::endl;
    }
    bVector = mat * bVector;
    std::cerr << "BVector is " << bVector << std::endl;
    
    // Use the bump vector to perturb the normal
    normal += bVector;
    normal.normalize();
    
  }

}
#else // NEW WAY

void Primitive::normal(Vector& normal,
		       RenderContext& context,
		       const Point& hitpos,
		       const Ray& ray,
		       const HitRecord& hit) const
{
  // Normal comes in in World Space
  
  getNormal( normal, context, hitpos, ray, hit );
  if ( _bump ) {
    Vector bVector = _bump->getBump( context, ray, hit );
    //PRINT_TIMES("BVector before is "<< bVector, 10);
    Matrix4 mat4 = Matrix3::AlignVectorWithAxis( normal,
						 Matrix3::Z_AXIS );
    mat4 = mat4.invert();
    bVector = ((Matrix3)mat4) * bVector;
    //PRINT_TIMES("BVector is " << bVector, 10);
    
    // Use the bump vector to perturb the normal
    normal += bVector;
    normal.normalize();
    
  }

}
#endif
