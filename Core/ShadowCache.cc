#include "Core/ShadowCache.h"

const unsigned int MAX_CACHE_SIZE = 3; // 5 primitives in the cache.

void
ShadowCache::addPrimitive( const Primitive * primitive ) {
  _list.push_front(primitive);
  if ( _list.size() > MAX_CACHE_SIZE )
    _list.pop_back();
}

