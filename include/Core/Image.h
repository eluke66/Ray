
#ifndef Image_h
#define Image_h

#include "Core/Color.h"
#include "Core/Math.h"
#include <string>

struct Pixel {
  unsigned long r, g, b;
};

class Image {
 public:
  Image(const std::string& name);
  Image(int _xres, int yres);
  ~Image();
  void set(int x, int y, const Pixel&p ) {
    assert( x < _xres );
    assert( y < _yres );
    assert(_data[x]);
    _data[x][y] = p;
  }
  void set(int x, int y, const Color& c) {
    Pixel p;
    assert( x < _xres );
    assert( y < _yres );
    p.r = c.r() < 0.f? 0: c.r() >= 1.f? 255 : (int)(c.r() * 255.f);
    p.g = c.g() < 0.f? 0: c.g() >= 1.f? 255 : (int)(c.g() * 255.f);
    p.b = c.b() < 0.f? 0: c.b() >= 1.f? 255 : (int)(c.b() * 255.f);
    _data[x][y] = p;
  }
  Color getPixel(int x, int y) const {
    Pixel &p = _data[x][y];
    return Color(p.r / 255.0,
		 p.g / 255.0,
		 p.b / 255.0);

  }
  void write(const std::string& filename) const;
  double aspect_ratio() const {
    return double(_xres)/double(_yres);
  }
  int width() const { return _xres; }
  int height() const { return _yres; }
  int getXresolution() {
    return _xres;
  }
  int getYresolution() {
    return _yres;
  }
  
  typedef void (*DeallocateFunction)(Pixel **, int);
  void setRaster( Pixel ** data, int xres, int yres, DeallocateFunction dealloc);
  Pixel ** getRaster() { return _data; }
  void allocateImage();
  void deallocateImage();

  Color interpolate(float x, float y) const {
    x *= _xres; y *= _yres;
    int ix = Floor(x)%_xres;
    if(ix<0)
      ix += _xres;
    int ix1 = (ix+1)%_xres;
    int iy = Floor(y)%_yres;
    if(iy<0)
      iy += _yres;
    int iy1 = (iy+1)%_yres;
    float fx = x-ix;
    float fy = y-iy;

    Color c00 = Color(_data[ix][iy].r,   _data[ix][iy].g,   _data[ix][iy].b);
    Color c01 = Color(_data[ix][iy1].r,  _data[ix][iy1].g,  _data[ix][iy1].b);
    Color c10 = Color(_data[ix1][iy].r,  _data[ix1][iy].g,  _data[ix1][iy].b);
    Color c11 = Color(_data[ix1][iy1].r, _data[ix1][iy1].g, _data[ix1][iy1].b);
    Color c = c00*(1-fx)*(1-fy) + c01*fx*(1-fy) + c10*(1-fx)*fy + c11*fx*fy;
    return c*(1./255);
  }
 protected:
  Pixel** _data;
  int _xres, _yres;
  DeallocateFunction _deallocator;
  Image(const Image&);
  Image& operator=(const Image&);
  
};

#endif
