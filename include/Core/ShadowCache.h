#ifndef ShadowCache_h
#define ShadowCache_h

class Primitive;
#include <list>


class ShadowCache {
public:

  typedef std::list<const Primitive *> PrimitiveList;
  
  const PrimitiveList& getPrimitives() { return _list; }
  void addPrimitive( const Primitive * primitive );
  
private:
  PrimitiveList _list;
};

#endif
