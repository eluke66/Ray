
#ifndef CloudBackground_h
#define CloudBackground_h

#include "Background/UVWBackground.h"
#include "Core/Color.h"
#include "Material/CloudColor.h"
#include "Core/Ray.h"

template <typename BGType, typename CloudType>
class CloudBackground : public UVWBackground {
 public:
  // Cloudiness goes from 0 -> 1.
  CloudBackground(const BGType &background,
		  const CloudType &cloudColor,
		  double cloudiness) : _cc(background,
					   cloudColor,
					   cloudiness)
  {}

  virtual void getBackgroundColor(Color& result,
				  const RenderContext&,
				  const Ray& ray)
    {
      Vector uvw;
      getUVW( uvw, ray.direction() );
      (_cc)(result, uvw);
    }


private:
  CloudBackground(const CloudBackground&);
  CloudBackground& operator=(const CloudBackground&);
  CloudColor<BGType,CloudType> _cc;
};

#endif

