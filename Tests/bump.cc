#include "Geometry/Primitive.h"
#include "Geometry/Plane.h"
#include "Geometry/Bump.h"
#include "Core/RenderContext.h"
#include "Core/Ray.h"
#include "Core/utils.h"
#include "Core/HitRecord.h"
#include "Core/Scene.h"

class BumpPrimitive : public Primitive {
public:
  
  BumpPrimitive(Bump *bump, const Vector &normal) :
    Primitive(NULL,bump),
    _plane( NULL, normal, Point(0,0,0) )
  {}
  virtual void intersect(HitRecord& hit,
			 const RenderContext& context,
			 const Ray& ray) const {
    _plane.intersect(hit,context,ray);
  }
  virtual void transform(const Matrix4 &matrix){_plane.transform(matrix);}
  virtual void getBounds(BoundingBox& bbox) const{_plane.getBounds(bbox);}
protected:
  virtual void getNormal(Vector& normal,
			 RenderContext& context,
			 const Point& hitpos,
			 const Ray& ray,
			 const HitRecord& hit) const {
    _plane.getNormal( normal, context, hitpos, ray, hit );
  }
  Plane _plane;
};

static Scene scene;

void test1() {
  ConstantBump bump( Vector(0,0,1) );
  BumpPrimitive b(&bump, Vector(0,0,1));
  RenderContext context(&scene);
  HitRecord hit(DBL_MAX);
  Ray ray( Point( 0, 0, 10 ), Vector( 0,0,-1) );
  b.intersect( hit, context, ray );
  assert( hit.getPrimitive() );

  Vector normal;
  Point hitpos = ray.origin() + hit.minT() * ray.direction();
  b.normal(normal,context, hitpos, ray, hit );

  assert( (Point)normal == Point(0,0,1) );
  std::cerr << " test 1 okay" << std::endl;
}

void test2() {
  ConstantBump bump( Vector(0,0,1) );
  BumpPrimitive b(&bump, Vector(0.707,0,0.707));
  RenderContext context(&scene);
  HitRecord hit(DBL_MAX);
  Ray ray( Point( 0, 0, 10 ), Vector( 0,0,-1) );
  b.intersect( hit, context, ray );
  assert( hit.getPrimitive() );

  Vector normal;
  Point hitpos = ray.origin() + hit.minT() * ray.direction();
  b.normal(normal,context, hitpos, ray, hit );
  std::cerr << " test 2 okay" << std::endl;
}

void test3() {
  ConstantBump bump( Vector(0,0.707,0.707) );
  BumpPrimitive b(&bump, Vector(0,0,1));
  RenderContext context(&scene);
  HitRecord hit(DBL_MAX);
  Ray ray( Point( 0, 0, 10 ), Vector( 0,0,-1) );
  b.intersect( hit, context, ray );
  assert( hit.getPrimitive() );

  Vector normal;
  Point hitpos = ray.origin() + hit.minT() * ray.direction();
  b.normal(normal,context, hitpos, ray, hit );

  Vector nv = Vector(0,0.707,0.707) + Vector(0,0,1);
  nv.normalize();

  assert( (Point)normal == (Point)nv );
  std::cerr << " test 3 okay" << std::endl;
}

void test4() {
  ConstantBump bump( Vector(0,0,1) );
  BumpPrimitive b(&bump, Vector(0,1,0));
  RenderContext context(&scene);
  HitRecord hit(DBL_MAX);
  Ray ray( Point( 0, 10, 0 ), Vector( 0,-1,0) );
  b.intersect( hit, context, ray );
  assert( hit.getPrimitive() );

  Vector normal;
  Point hitpos = ray.origin() + hit.minT() * ray.direction();
  b.normal(normal,context, hitpos, ray, hit );
  std::cerr << normal << std::endl;

  assert( (Point)normal == Point(0,1,0) );
  std::cerr << " test 4 okay" << std::endl;
}

void test5() {
  ConstantBump bump( Vector(0,0,1) );
  BumpPrimitive b(&bump, Vector(1,0,0));
  RenderContext context(&scene);
  HitRecord hit(DBL_MAX);
  Ray ray( Point( 10, 0, 0 ), Vector( -1,0,0) );
  b.intersect( hit, context, ray );
  assert( hit.getPrimitive() );

  Vector normal;
  Point hitpos = ray.origin() + hit.minT() * ray.direction();
  b.normal(normal,context, hitpos, ray, hit );
  std::cerr << normal << std::endl;

  assert( (Point)normal == Point(1,0,0) );
  std::cerr << " test 5 okay" << std::endl;
}

void test6() {
  ConstantBump bump( Vector(0,0.707,0.707) );
  BumpPrimitive b(&bump, Vector(0,0.707,0.707));
  RenderContext context(&scene);
  HitRecord hit(DBL_MAX);
  Ray ray( Point( 0, 0, 10 ), Vector( 0,0,-1) );
  b.intersect( hit, context, ray );
  assert( hit.getPrimitive() );

  Vector normal;
  Point hitpos = ray.origin() + hit.minT() * ray.direction();
  b.normal(normal,context, hitpos, ray, hit );

  std::cerr << "normal: "<< normal << std::endl;
  std::cerr << " test 6 okay" << std::endl;
}

int
main() {

  test1();
  test2();
  test3();
  test4();
  test5();
  test6();
}
