#ifndef Material_h_
#define Material_h_

class Color;
class HitRecord;
class Ray;

#include "Core/RenderContext.h"
#include "Core/Statistics.h"

//#define DEBUG_MAT_NAME
#ifdef DEBUG_MAT_NAME
#include "Core/Backtrace.h"
#endif

class NoOp {
public:
  void operator()(Color&,RenderContext&,const Ray&,
		  const HitRecord&,const Color&, int) const {}
  bool bounce( Ray&, bool&, double, const HitRecord &, const Ray &) const { return false; }
  bool generatesCaustics() const { return false; }
  static NoOp None;
};

#include <iostream>
class Material {
public:
  // Type of ray bounces.
  typedef enum { NONE, DIFFUSE, SPECULAR } BounceType;
  
  virtual ~Material() { std::cerr << "~Material" << std::endl;}
  virtual void shade(Color& result, RenderContext& context, const Ray& ray,
		     const HitRecord& hit, const Color& atten, int depth) const = 0;
  virtual BounceType bounceRay( Ray &newDirection, Color &newColor, bool &diffuse, const HitRecord &hit, const Ray &incoming, RenderContext &context ) const = 0;

#ifdef DEBUG_MAT_NAME
  Material() {
    _matName = NULL;
    int numFrames;
    char ** bt = getBacktrace(numFrames);

    unsigned totalSize = 0;
    int i;
    for ( i = 1; i < numFrames; ++i ) {
      totalSize += strlen(bt[i]) + strlen(" : ");
    }
    _matName = new char[totalSize + 1];
    char * current = _matName;
    char * endOfMat = _matName + totalSize + 1;
    for ( i = 1; i < numFrames; ++i ) {
      assert( current < endOfMat );
      strncpy(current, bt[i], strlen(bt[i]));
      //std::cerr << "\tInc by " << strlen(bt[i]) << " (" << bt[i] << ") Current= " << (current + strlen(bt[i]) - _matName) << std::endl;
      current += strlen(bt[i]);

      strncpy(current, " : ", strlen(" : "));
      current += strlen(" : ");

      delete[] bt[i];
    }
    *current = 0;
    delete[] bt;
    //std::cerr << "MAT " << _matName << std::endl;
  }
  char * _matName;
#endif
};


#include "Material/GlobalIllumMat.h"
#include <stdlib.h>
template <typename Coloring,
	  typename Lighting=NoOp,
	  typename Dielectric=NoOp,
	  typename GlobalIllumination=NoOp>
class _Material : public Material {
public:
  _Material( const Coloring &coloring,
	    const Lighting &lighting=NoOp::None,
	    const Dielectric &dielectric=NoOp::None,
	    const GlobalIllumination &illum=NoOp::None ) :
    _coloring(coloring),
    _lighting(lighting),
    _dielectric(dielectric),
    _globalIllumination(illum) {
    ALLOC;
  }
  virtual ~_Material() {}
  void shade(Color& result, RenderContext& context, const Ray& ray,
	     const HitRecord& hit, const Color& atten, int depth) const {
    _coloring(result,context,ray,hit,atten,depth);
    context.pigment = result;
    _dielectric(result,context,ray,hit,atten,depth);
    _lighting(result,context,ray,hit,atten,depth);
    _globalIllumination(result,context,ray,hit,atten,depth);
  }

  virtual BounceType bounceRay( Ray &newDirection, Color &newColor,
				bool &diffuse, const HitRecord &hit,
				const Ray &incoming, RenderContext &context ) const
  {
    /*
      Algorithm:
      Get a random number.
      See if the dielectric affects the photon.
      If not, then try the lighting.
     */
    BounceType bounceType = DIFFUSE;
    double random = drand48();
    bool reflected = _dielectric.bounce( newDirection,
					 diffuse,
					 random,
					 hit,
					 incoming );

    // If not reflected, try the lighting. This may cause the photon
    // to be stored as well.
    if ( !reflected ) {
      _lighting.bounce( newDirection,
			diffuse,
			random,
			hit,
			incoming );
    }
    else
      bounceType = SPECULAR;

    // Find new color of the photon.
    _coloring(newColor,context,incoming,hit,Color(1.0),1);

    // We use Russian Roulette based on the power of the photon to
    // kill or propagate the photon.
    if ( reflected || (newColor.power() > drand48()) )
      return bounceType;
    
    return NONE;
  }
  
private:
  Coloring           _coloring;
  Lighting           _lighting;
  Dielectric         _dielectric;
  GlobalIllumination _globalIllumination;

  
  _Material(const _Material &);
  _Material& operator=(const _Material&);
  
};

#endif
