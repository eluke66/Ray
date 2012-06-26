
#include "Geometry/Group.h"
#include "Core/BoundingBox.h"
#include "Core/Matrix.h"
#include <iostream>

using namespace std;

#define ERIC_MULTITHREAD_PREPROCESS
#ifdef ERIC_MULTITHREAD_PREPROCESS
#include "Core/Thread.h"
#include "Core/Options.h"

class ObjectHolder {
public:
  ObjectHolder( std::vector<Object *> &objects ) : _objects(objects), _which(0), _mutex("Object Locker")
  {
    AlreadyParallel = true;
  }
  ~ObjectHolder() {
    AlreadyParallel = false;
  }

  static bool IsParallel() { return !AlreadyParallel; }
  
  Object *nextObject() {
    MutexLocker lock(_mutex);
    if ( _which < _objects.size() ) {
      return _objects[_which++];
    }
    return NULL;
  }
    
private:
  ObjectHolder();
  std::vector<Object *> &_objects;
  unsigned int _which;
  Mutex _mutex;
  static bool AlreadyParallel;
};

bool
ObjectHolder::AlreadyParallel = false;

class PreprocessThread : public Thread {
public:
  PreprocessThread( ObjectHolder * holder ) : Thread("PreprocessingThread"), _holder(holder) {
    NumPPThreads++;
  }
  virtual ~PreprocessThread() {
    
  }
  
  virtual void *run() {

    assert( _holder );

    Object * object;
    
    // Run forever
    while ( 1 ) {
      object = _holder->nextObject();
      if ( !object )
	break;
      object->preprocess();
    }
    --NumPPThreads;
    Cond.signal();
    return NULL;
  }

  static void WaitForAll() {
    while ( NumPPThreads > 0 ) {
      Cond.wait();
    }
    
  }
  
private:
  PreprocessThread();
  ObjectHolder *_holder;
  static int NumPPThreads;
  static Mutex Waiter;
  static Condition Cond;
};

Mutex
PreprocessThread::Waiter("PP Waiter");

Condition
PreprocessThread::Cond( PreprocessThread::Waiter );

int
PreprocessThread::NumPPThreads = 0;

#endif // ERIC_MULTITHREAD_PREPROCESS


Group::Group() : _haveBounds(false)
{
  ALLOC;
}

Group::~Group()
{
}

void Group::addObject(Object* object)
{
  objects.push_back(object);
}

void Group::setMaterial(Material *mat, bool override) {
  Object** begin = &objects[0];
  Object** end = &objects[objects.size()];
  while (begin != end) {
    (*begin++)->setMaterial(mat, override);
  }
}

void Group::transform(const Matrix4 &matrix) {
  Object** begin = &objects[0];
  Object** end = &objects[objects.size()];
  while (begin != end)
    (*begin++)->transform(matrix);
}

void Group::preprocess()
{
#ifdef ERIC_MULTITHREAD_PREPROCESS
  if ( ObjectHolder::IsParallel() ) {
    ObjectHolder o( objects );
    for ( unsigned i = 0; i < Options::Instance()->numThreads(); ++i )
      (new PreprocessThread( &o ))->start(true);
    PreprocessThread::WaitForAll();
  }
  else {
    Object*const* begin = &objects[0];
    Object*const* end = &objects[objects.size()];
    while (begin != end)
      (*begin++)->preprocess();
  }
#else
  {
    Object*const* begin = &objects[0];
    Object*const* end = &objects[objects.size()];
    while (begin != end)
      (*begin++)->preprocess();
  }
#endif

  Object*const* begin = &objects[0];
  Object*const* end = &objects[objects.size()];
  while (begin != end)
    (*begin++)->getBounds(_bbox);
  _haveBounds = true;
}

void Group::getBounds(BoundingBox& bbox) const
{
  if (!_haveBounds) {
    Object*const* begin = &objects[0];
    Object*const* end = &objects[objects.size()];
    while (begin != end)
      (*begin++)->getBounds(bbox);
  }
  else {
    bbox.extend(_bbox.getMin());
    bbox.extend(_bbox.getMax());
  }
}

void Group::intersect(HitRecord& hit, const RenderContext& context, const Ray& ray) const
{
  Object*const* begin = &objects[0];
  Object*const* end = &objects[objects.size()];
  while (begin != end)
    (*begin++)->intersect(hit, context, ray);
}
