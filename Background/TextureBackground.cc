#include "Background/TextureBackground.h"
#include "Material/TextureColor.h"
#include "Core/Ray.h"


TextureBackground::TextureBackground(const std::string &textureName )
  : UVWBackground( Vector(0,0,1), Vector(0,1,0) ),
    _mat( textureName )
{
}

TextureBackground::~TextureBackground()
{
}

void TextureBackground::preprocess()
{
}

void TextureBackground::getBackgroundColor(Color& result,
					   const RenderContext&context,
					   const Ray&ray)
{
  Vector uvw;
  getUVW( uvw, ray.direction() );
  (_mat)( result, uvw );
}
