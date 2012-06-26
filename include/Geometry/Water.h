
#ifndef Water_h
#define Water_h

#include "Geometry/Heightfield.h"

class Water : public Heightfield {
 public:
  Water(Material* material,
	int resolution, // must be a power of 2
	bool choppy,    // Is the water choppy?
	bool bestNormals, // Use fft to get normals
	float windDirection, // In radians,
	float a, // Affects wave height
	float v, // Wind velocity
	const Point &p1,
	const Point &p2);
	
  virtual ~Water();


};

#endif
