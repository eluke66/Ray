#include "Background/GradientBackground.h"

#include "Core/Ray.h"

#include <float.h>

GradientBackground::GradientBackground( const Color& tr, const Color &tl,
					bool horizontal, float repeat )
  : UVWBackground( Vector(0,0,1), Vector(0,1,0) ),
    _mat(tr,tl,horizontal,repeat)
{
}

GradientBackground::GradientBackground( const Color& tr, const Color &tl,
					const Color& br, const Color &bl,
					float hRepeat, float vRepeat )
  : UVWBackground( Vector(0,0,1), Vector(0,1,0) ),
    _mat(tr,tl,br,bl,hRepeat,vRepeat)
{
}

GradientBackground::~GradientBackground()
{
}

void GradientBackground::preprocess()
{
}

void GradientBackground::getBackgroundColor(Color& result,
					   const RenderContext&,
					   const Ray&ray)
{
  Vector uvw;
  getUVW( uvw, ray.direction() );
  (_mat)( result, uvw );
}


MultiGradientBackground::MultiGradientBackground( const MultiGradientColor &mg )
  : UVWBackground( Vector(0,0,1), Vector(0,1,0) ),
    _mat(mg)
{
}

void MultiGradientBackground::preprocess()
{
}

void MultiGradientBackground::getBackgroundColor(Color& result,
					   const RenderContext&,
					   const Ray&ray)
{
  Vector uvw;
  getUVW( uvw, ray.direction() );
  (_mat)( result, uvw );
}
