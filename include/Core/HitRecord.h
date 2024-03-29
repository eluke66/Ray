
#ifndef HitRecord_h
#define HitRecord_h

class Material;
class Primitive;
#define EPSILON (1.e-6)
#include <float.h>

class HitRecord {
 public:
  HitRecord(double max=DBL_MAX) {
    prim = 0;
    realPrim = 0;
    matl = 0;
    min = max;
  }
  ~HitRecord() {
  }
  bool hit(double t, const Primitive* hit_prim, const Material* hit_matl) {
    if(t > EPSILON && t < min){
      min = t;
      prim = hit_prim;
      matl = hit_matl;
      return true;
    } else {
      return false;
    }
  }
  double minT() const {
    return min;
  }
  const Primitive* getPrimitive() const {
    return prim;
  }
  const Material* getMaterial() const {
    return matl;
  }
  void setMaterial( const Material * matl ) {
    this->matl = matl;
  }
  void setPrimitive( const Primitive * prim ) {
    this->prim = prim;
  }
  template< typename T >
    T* getScratchpad()
  {
    char unnamed[ ( sizeof( T ) <= sizeof( scratchdata ) ) ? 1 : 0 ];
    return reinterpret_cast< T * >( scratchdata );
  }
  template< typename T >
    const T* getScratchpad() const
  {
    char unnamed[ ( sizeof( T ) <= sizeof( scratchdata ) ) ? 1 : 0 ];
    return reinterpret_cast< const T * >( scratchdata );
  }

  const Primitive *realPrim;
 private:
  const Primitive* prim;
  const Material* matl;
  double min;

  char scratchdata[128];
};

#endif
