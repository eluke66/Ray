#ifndef VolumeReader_h_
#define VolumeReader_h_

#include <string>
#include "Core/Array3.h"

class Vector;

class VolumeReader {
public:
  static void ReadVolume( const std::string &headername,
			  const Vector &diag,
			  double grid_stepsize,
			  double &world_stepsize, Vector &cellsize, Array3<short> &data );
private:
  VolumeReader();
};

#endif
