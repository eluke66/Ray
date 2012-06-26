
#include "Geometry/Heightfield.h"
#include "Core/BoundingBox.h"
#include "Core/HitRecord.h"
#include "Core/Math.h"
#include "Core/Point.h"
#include "Core/Ray.h"
#include "Core/Vector.h"
#include "Core/Matrix.h"
#include "Core/utils.h"

#include <iostream>
#include <fstream>
#include <float.h>
using namespace std;

Heightfield::Heightfield(Material* material, const std::string& filename,
                         const Point& inp1, const Point& inp2)
  : Primitive(material), p1(inp1), p2(inp2)
{
  ALLOC;
  ifstream in(filename.c_str());
  if(!in){
    cerr << "Error opening " << filename << "\n";
    exit(1);
  }
  double minz, maxz;
  in >> nx >> ny >> minz >> maxz;
  if(!in){
    cerr << "Error reading hader from " << filename << "\n";
    exit(1);
  }
  in.get();
  data = new float*[nx+1];
  float* p = new float[(nx+1)*(ny+1)];
  for(int i=0;i<=nx;i++)
    data[i] = p+i*(ny+1);
  in.read(reinterpret_cast<char*>(data[0]), sizeof(float)*(nx+1)*(ny+1));
  
  if(!in){
    cerr << "Error reading data from " << filename << "\n";
    exit(1);
  }
  setData( minz, maxz );
}

Heightfield::Heightfield(Material *material,
			 const Point &p1, const Point &p2) :
  Primitive(material), p1(p1), p2(p2), data(NULL), nx(-1), ny(-1)
{
  ALLOC;
}

Heightfield::~Heightfield()
{
}

void Heightfield::setData( double minz, double maxz) {
  double dz = maxz-minz;
  if(dz < 1.e-3)
    dz = 1.e-3;
  p1 = Point(p1.x(), p1.y(), minz-dz*1.e-4);
  p2 = Point(p2.x(), p2.y(), maxz+dz*1.e-4);
  
  // Step 1
  diag = p2-p1;
  cellsize = diag/Vector(nx, ny, 1);
  inv_cellsize = cellsize.inverse();
}

void Heightfield::transform(const Matrix4 &matrix) {
  p1 = matrix * p1;
  p2 = matrix * p2;
  diag = p2-p1;
  cellsize = diag/Vector(nx, ny, 1);
  inv_cellsize = cellsize.inverse();
}

float Heightfield::getMinimum() const {
  
  float vmin = FLT_MAX;
  
  for ( int i = 0; i < nx; ++i )
    for ( int j = 0; j < ny; ++j ) {
      vmin = std::min(vmin, data[i][j] );
    }
  return vmin;
  
}

void Heightfield::getBounds(BoundingBox& bbox) const
{
  bbox.extend(p1);
  bbox.extend(p2);
}

void Heightfield::computeUVW(Vector& uvw, const RenderContext& context,
			     const Ray& ray, const HitRecord& hit) const
{
  Point hitpos = ray.origin()+ray.direction()*hit.minT();

  uvw = (hitpos - p1) / (p2 - p1);
  //  assert( IN_RANGE(uvw.x(), 1.0, 0.0, 1e-6) );
  // assert( IN_RANGE(uvw.y(), 1.0, 0.0, 1e-6) );
  
}

void Heightfield::intersect(HitRecord& hit, const RenderContext&, const Ray& ray) const
{
  // Step 2
  Vector idir = ray.inverseDirection();
  Vector v1 = (p1-ray.origin())*idir;
  Vector v2 = (p2-ray.origin())*idir;
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
  int Lx = (int)((p.x()-p1.x())*inv_cellsize.x());
  Lx = Lx<0?0:Lx>=nx?nx-1:Lx;
  int Ly = (int)((p.y()-p1.y())*inv_cellsize.y());
  if(Ly < 0) Ly = 0;
  else if(Ly >= ny) Ly = ny-1;

  // Step 4
  double signx = diag.x()*ray.direction().x();
  double signy = diag.y()*ray.direction().y();
  int dix = signx>0?1:-1;
  int stopx = signx>0?nx:-1;
  int diy = signy>0?1:-1;
  int stopy = signy>0?ny:-1;

  // Step 5
  double dtdx = Abs(cellsize.x()/ray.direction().x());
  double dtdy = Abs(cellsize.y()/ray.direction().y());

  // Step 6
  double far_x;
  if(signx>0)
    far_x = (Lx+1)*cellsize.x() + p1.x();
  else
    far_x = Lx*cellsize.x() + p1.x();
  double far_y;
  if(signy>0)
    far_y = (Ly+1)*cellsize.y() + p1.y();
  else
    far_y = Ly*cellsize.y() + p1.y();

  // Step 7
  double tnext_x = (far_x - ray.origin().x())/ray.direction().x();
  double tnext_y = (far_y - ray.origin().y())/ray.direction().y();

  // Step 8
  float zenter = ray.origin().z() + tnear * ray.direction().z();
  tfar = Min(tfar, hit.minT());
  while(tnear < tfar){
    double texit = Min(tnext_x, tnext_y);
    float zexit = ray.origin().z() + texit * ray.direction().z();

    // Step 9
    float datamin = Min(data[Lx][Ly], data[Lx+1][Ly], data[Lx][Ly+1], data[Lx+1][Ly+1]);
    float datamax = Max(data[Lx][Ly], data[Lx+1][Ly], data[Lx][Ly+1], data[Lx+1][Ly+1]);
    float zmin = Min(zenter, zexit);
    float zmax = Max(zenter, zexit);
    if(zmin < datamax && zmax > datamin){
      // Step 10
      Point C = p1 + Vector(Lx, Ly, 0)*cellsize;
      Vector EC = ray.origin()+tnear*ray.direction()-C;
      double Ex = EC.x()*inv_cellsize.x();
      double Ey = EC.y()*inv_cellsize.y();
      double Ez = ray.origin().z()+tnear*ray.direction().z();
      double Vx = ray.direction().x()*inv_cellsize.x();
      double Vy = ray.direction().y()*inv_cellsize.y();
      double Vz = ray.direction().z();
      float za = data[Lx][Ly];
      float zb = data[Lx+1][Ly]-za;
      float zc = data[Lx][Ly+1]-za;
      float zd = data[Lx+1][Ly+1]-zb-zc-za;
      double a = Vx*Vy*zd;
      double b = -Vz + Vx*zb + Vy*zc + (Ex*Vy + Ey*Vx)*zd;
      double c = -Ez + za + Ex*zb + Ey*zc + Ex*Ey*zd;
      if(Abs(a) < 1.e-6){
        // Linear
        double tcell = -c/b;
        if(tcell > 0 && tnear+tcell < texit){
          if(hit.hit(tnear+tcell, this, matl)){
            HitData* hd = hit.getScratchpad<HitData>();
            hd->x = Lx;
            hd->y = Ly;
            return;
          }
        }
      } else {
        // Solve quadatric
        double disc = b*b-4*a*c;
        if(disc > 0){
          double root = sqrt(disc);
          double tcell1 = (-b + root)/(2*a);
          double tcell2 = (-b - root)/(2*a);
          if(tcell1 >= 0 && tnear+tcell1 <= texit){
            if(hit.hit(tnear+tcell1, this, matl)){
              if(tcell2 >= 0)
                // Check the other root in case it is closer.
                // No need for an additional if() because it will still
                // be in the same cell
                hit.hit(tnear+tcell2, this, matl);
              HitData* hd = hit.getScratchpad<HitData>();
              hd->x = Lx;
              hd->y = Ly;
              return;
            }
          } else if(tcell2 >= 0 && tnear+tcell2 <= texit){
            if(hit.hit(tnear+tcell2, this, matl)){ 
              HitData* hd = hit.getScratchpad<HitData>();
              hd->x = Lx;
              hd->y = Ly;
              return;
            }
          }
        }
      }
    }
    // Step 11
    zenter = zexit;
    if(tnext_x < tnext_y){
      Lx += dix;
      if(Lx == stopx)
        break;
      tnear = tnext_x;
      tnext_x += dtdx;
    } else {
      Ly += diy;
      if(Ly == stopy)
        break;
      tnear = tnext_y;
      tnext_y += dtdy;
    }
  }
}

void Heightfield::getNormal(Vector& normal, RenderContext&, const Point& hitpos,
                    const Ray& ray, const HitRecord& hit) const
{
  const HitData* hd = hit.getScratchpad<HitData>();
  int Lx = hd->x;
  int Ly = hd->y;
  Point C = p1 + Vector(Lx, Ly, 0)*cellsize;
  double u = (hitpos.x()-C.x())*inv_cellsize.x();
  double v = (hitpos.y()-C.y())*inv_cellsize.y();
  double dudx = inv_cellsize.x();
  double dvdy = inv_cellsize.y();
  float za = data[Lx][Ly];
  float zb = data[Lx+1][Ly]-za;
  float zc = data[Lx][Ly+1]-za;
  float zd = data[Lx+1][Ly+1]-zb-zc-za;
  double px = dudx*zb + dudx*v*zd;
  double py = dvdy*zc + dvdy*u*zd;
  normal = Vector(-px, -py, 1);
  normal.normalize();
}
