#include "Material/TextureColor.h"

#include "Core/HitRecord.h"
#include "Core/Vector.h"
#include "Geometry/Primitive.h"
#include "IO/ImageLoader.h"
#include <iostream>

TextureColor::TextureColor(const std::string& imagename, double uRepeat, double vRepeat) :
  _uRepeat(uRepeat), _vRepeat(vRepeat)
{
  
  _image = ImageLoader::LoadImage(imagename);
  assert(_image);
}

TextureColor::TextureColor(const TextureColor& other, double uRepeat, double vRepeat) :
  _image( other._image ), _uRepeat(uRepeat), _vRepeat(vRepeat)
{
  assert( _image );
}

TextureColor::TextureColor(Image *image, double uRepeat, double vRepeat):
  _image( image ), _uRepeat(uRepeat), _vRepeat(vRepeat)
{
  assert( _image );
}

void TextureColor::operator()(Color& result, RenderContext& context, const Ray& ray,
			      const HitRecord& hit, const Color& atten, int depth) const {

  Vector uvw;
  hit.getPrimitive()->computeUVW(uvw, context, ray, hit);


  (*this)(result, uvw);
}

void
TextureColor::operator()(Color& result, const Vector &uvw ) const {
  double x = uvw.x() * _uRepeat;
  double y = uvw.y() * _vRepeat;

#ifdef TEST_EJ
  result = _image->interpolate(x - floor(x),
			       y - floor(y));
#else
  int _xres = _image->width();
  int _yres = _image->height();
  x *= _xres; y *= _yres;
  int ix = Floor(x)%_xres;
  if(ix<0)
    ix += _xres;
  int iy = Floor(y)%_yres;
  if(iy<0)
    iy += _yres;
  result = _image->getPixel(ix,iy);
#endif
}
