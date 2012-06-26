#include "Material/MaterialModulator.h"
#include "Material/Modulators.h"
#include "Core/Ray.h"
#include "Core/Color.h"


void MaterialModulator::shade(Color& result,
			      RenderContext& context,
			      const Ray& ray,
			      const HitRecord& hit,
			      const Color& atten,
			      int depth) const {


  std::vector<double> weights;
  Color c1;
  
  _modulator->getWeights(weights, context, ray, hit );
  
  unsigned numMats = std::min( weights.size(), _materials.size() );
  for (unsigned i = 0; i < numMats; ++i ) {
    _materials[i]->shade( c1, context, ray,
			  hit, atten*weights[i], depth);
    result += c1 * weights[i];
  }
}

Material::BounceType
MaterialModulator::bounceRay( Ray &newDirection, Color &newColor,
			      bool &diffuse,
			      const HitRecord &hit,
			      const Ray &incoming,
			      RenderContext &context ) const
{
  std::vector<double> weights;
  
  _modulator->getWeights(weights, context, incoming, hit );

  Color c1;
  Vector newDir;
  Ray newRay;
  double bounceProbability = 0.0;
  double diffuseProbability = 0.0;
  bool d;
  unsigned numMats = std::min( weights.size(), _materials.size() );
  for (unsigned i = 0; i < numMats; ++i ) {
    if ( weights[i] > 0.0 ) {
      bool bounced =
	_materials[i]->bounceRay( newRay, c1, d, hit, incoming, context );
      if ( bounced )
	bounceProbability += weights[i];
      if ( d )
	diffuseProbability += weights[i];
      newDir += newRay.direction() * weights[i];
      newColor += c1 * weights[i];
    }
  }

  // Now, set diffuse and bounce.
  diffuse = (diffuseProbability > 0.5);
  bool bounces = (bounceProbability > 0.5);
  
  if ( bounces )
    if ( diffuse )
      return DIFFUSE;
    else
      return SPECULAR;

  return NONE;
}
