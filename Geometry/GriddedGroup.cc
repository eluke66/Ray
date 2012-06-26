
#include "Geometry/GriddedGroup.h"
#include "Core/BoundingBox.h"
#include "Core/HitRecord.h"
#include "Core/Ray.h"
#include "Core/Time.h"
#include "Core/Options.h"
#include <iostream>
#include <typeinfo>
using namespace std;

#define NO_VERBOSE

GriddedGroup::GriddedGroup(double cellfactor)
  : cellfactor(cellfactor)
{
  ALLOC;
  DALLOC(Group);
}

GriddedGroup::~GriddedGroup()
{
}

void GriddedGroup::transformToLattice(const BoundingBox& box,
                                      int& sx, int& sy, int& sz,
                                      int& ex, int& ey, int& ez) const
{
  Vector s = (box.getMin()-min)*inv_cellsize-Vector(1.e-6,1.e-6,1.e-6);
  sx = Floor(s.x()); sy = Floor(s.y()); sz = Floor(s.z());
  Vector e = (box.getMax()-min)*inv_cellsize+Vector(1.e-6,1.e-6,1.e-6);
  ex = Ceil(e.x()); ey = Ceil(e.y()); ez = Ceil(e.z());
  //std::cerr << "s = " << s << " e = " << e << std::endl;
}

void GriddedGroup::preprocess()
{
  double start = Time::currentSeconds();
  bool quiet = Options::Instance()->quiet();
  Group::preprocess();

  const Object*const* fast_objects = &objects[0];

  BoundingBox bbox;
  getBounds(bbox);
  //std::cerr << "Preprocessing: Bounds are " << bbox << std::endl;
  min = bbox.getMin();
  max = bbox.getMax();
  diag = bbox.getMax() - bbox.getMin();
  diag = Max(Vector(1.e-4,1.e-4,1.e-4), diag);
  min -= diag*1.e-5;
  max += diag*1.e-5;
  diag = max-min;

  double volume = diag.x()*diag.y()*diag.z();
  double vol3 = cbrt(volume);
  int numObjects = objects.size();
  int target_numcells = static_cast<int>(numObjects*cellfactor);
  double avgside = cbrt(static_cast<double>(target_numcells));
  int nx = static_cast<int>(diag.x()/vol3 * avgside + 0.8);
  int ny = static_cast<int>(diag.y()/vol3 * avgside + 0.8);
  int nz = static_cast<int>(diag.z()/vol3 * avgside + 0.8);
  nx = Max(1, nx); ny = Max(1, ny); nz = Max(1, nz);
  int totalcells = nx*ny*nz;
  cellsize = diag/Vector(nx, ny, nz);
  inv_cellsize = cellsize.inverse();
  cells.resize(nx, ny, nz, 1);
  //std::cerr << "CELLS: " << nx<< " " << ny << " " << nz << " total: " << totalcells << std::endl;
  //std::cerr << " cellsize: " << cellsize << std::endl;

#ifndef NO_VERBOSE
  if ( !quiet ) cerr << "1. Initializing grid\n";
#endif
  cells.initialize(0);

#ifndef NO_VERBOSE
  if ( !quiet ) cerr << "2. Count objects\n";
#endif
  for(int i=0;i<static_cast<int>(objects.size());i++){
    const Object* obj = fast_objects[i];
    assert(obj);
    
    BoundingBox objbox;
    obj->getBounds(objbox);
    //    std::cerr << "\t" << objbox << std::endl;
    int sx, sy, sz, ex, ey, ez;
    transformToLattice(objbox, sx, sy, sz, ex, ey, ez);
    if ( ey > ny || ez > nz ) {
      std::cerr << "Error in object " << i << " (" << typeid(*obj).name() << ") : " << std::endl;
      std::cerr << "\tBBOX: " << objbox << std::endl;
      std::cerr << "Grid is " << nx << " " << ny << " " << nz << std::endl;
      std::cerr << "S/E are " << sx << " " << sy << " " << sz << " -> " << ex << " " << ey << " " << ez << std::endl;
    }
    assert( ex <= nx );
    assert( ey <= ny );
    assert( ez <= nz );
    for(int x=sx;x<ex;x++){
      for(int y=sy;y<ey;y++){
        for(int z=sz;z<ez;z++){
          cells(x, y, z)++;
        }
      }
    }
  }
  
#ifndef NO_VERBOSE
  if ( !quiet ) cerr << "3. Count cells\n";
#endif
  int sum = 0;
  for(int i=0;i<totalcells;i++){
    int nobj = cells[i];
    cells[i] = sum;
    sum += nobj;
  }
  cells[totalcells] = sum;
#ifndef NO_VERBOSE
  if ( !quiet ) cerr << "4. Allocating " << sum << " list entries\n";
#endif
  lists = new const Object*[sum];

  Array3<int> offset(nx, ny, nz);
  offset.initialize(0);
#ifndef NO_VERBOSE
  if ( !quiet ) cerr << "5. Filling in lists\n";
#endif
  for(int i=0;i<static_cast<int>(objects.size());i++){
    const Object* obj = fast_objects[i];

    BoundingBox objbox;
    obj->getBounds(objbox);
    int sx, sy, sz, ex, ey, ez;
    transformToLattice(objbox, sx, sy, sz, ex, ey, ez);
    for(int x=sx;x<ex;x++){
      for(int y=sy;y<ey;y++){
        for(int z=sz;z<ez;z++){
          int start = cells(x, y, z);
          int cur = offset(x, y, z)++;
          lists[start+cur] = obj;
        }
      }
    }
  }
  
#ifndef NO_VERBOSE
  if ( !quiet ) cerr << "6. verifying grid\n";
#endif
  for(int x = 0;x < nx; x++){
    for(int y = 0;y < ny; y++){
      for(int z = 0;z < nz; z++){
        int idx = cells.getIndex(x,y,z);
        int n = cells[idx+1]-cells[idx];
        if(n != offset(x, y, z))
          cerr << "Grid initialization error at " << x << ", " << y << ", " << z << ": count is " << offset(nx, ny, nz) << ", but should be " << n << '\n';
      }
    }
  }
  
#ifndef NO_VERBOSE
  double dt = Time::currentSeconds()-start;

  if ( !quiet ) cerr << "Grid: " << numObjects << " objects, targeted cells: " << target_numcells << ", actual cells: " << totalcells << ", " << nx << "x" << ny << "x" << nz << ", built in " << dt << " seconds\n";
#endif
}

void GriddedGroup::intersect(HitRecord& hit, const RenderContext& context, const Ray& ray) const
{
  // Step 2
  Vector idir = ray.direction().inverse();
  Vector v1 = (min-ray.origin())*idir;
  Vector v2 = (max-ray.origin())*idir;
  Vector vmin = Min(v1, v2);
  Vector vmax = Max(v1, v2);
  double tnear = vmin.maxComponent();
  double tfar = vmax.minComponent();
  if(tnear >= tfar)
    return;
  if(tfar < 1.e-6)
    return;
  if(tnear < 0)
    tnear = 0;

  // Step 3
  Point p = ray.origin() + tnear * ray.direction();
  Vector L = (p-min)*inv_cellsize;
  int Lx = Clamp(static_cast<int>(L.x()), 0, cells.getNx()-1);
  int Ly = Clamp(static_cast<int>(L.y()), 0, cells.getNy()-1);
  int Lz = Clamp(static_cast<int>(L.z()), 0, cells.getNz()-1);

  // Step 4
  Vector sign = diag*ray.direction();
  int dix = sign.x()>0?1:-1;
  int stopx = sign.x()>0?cells.getNx():-1;
  int diy = sign.y()>0?1:-1;
  int stopy = sign.y()>0?cells.getNy():-1;
  int diz = sign.z()>0?1:-1;
  int stopz = sign.z()>0?cells.getNz():-1;

  // Step 5
  double dtdx = Abs(cellsize.x()/ray.direction().x());
  double dtdy = Abs(cellsize.y()/ray.direction().y());
  double dtdz = Abs(cellsize.z()/ray.direction().z());

  // Step 6
  double far_x;
  if(sign.x()>0)
    far_x = (Lx+1)*cellsize.x() + min.x();
  else
    far_x = Lx*cellsize.x() + min.x();
  double far_y;
  if(sign.y()>0)
    far_y = (Ly+1)*cellsize.y() + min.y();
  else
    far_y = Ly*cellsize.y() + min.y();
  double far_z;
  if(sign.z()>0)
    far_z = (Lz+1)*cellsize.z() + min.z();
  else
    far_z = Lz*cellsize.z() + min.z();

  // Step 7
  double tnext_x = (far_x - ray.origin().x())*idir.x();
  double tnext_y = (far_y - ray.origin().y())*idir.y();
  double tnext_z = (far_z - ray.origin().z())*idir.z();

  // Step 8
  while(tnear < hit.minT()){

    int idx = cells.getIndex(Lx, Ly, Lz);
    int l = cells[idx];
    int e = cells[idx+1];
    for(;l < e; l++){
      const Object* obj = lists[l];
      obj->intersect(hit, context, ray);
    }
    // Step 11
    if(tnext_x < tnext_y && tnext_x < tnext_z){
      Lx += dix;
      if(Lx == stopx)
        break;
      tnear = tnext_x;
      tnext_x += dtdx;
    } else if(tnext_y < tnext_z){
      Ly += diy;
      if(Ly == stopy)
        break;
      tnear = tnext_y;
      tnext_y += dtdy;
    } else {
      Lz += diz;
      if(Lz == stopz)
        break;
      tnear = tnext_z;
      tnext_z += dtdz;
    }
  }
}
