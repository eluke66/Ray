#ifndef PHOTON_MAP_H_
#define PHOTON_MAP_H_

#include "Core/Color.h"
#include "Core/Point.h"
#include "Core/Vector.h"
#include "Core/BoundingBox.h"

class Photon;
class NearestPhotons;

class PhotonMap {
public:
  PhotonMap( int maxPhotons, bool causticsOnly=false );
  ~PhotonMap();

  // Stores a photon
  void store( const Color &power,
	      const Point &pos,
	      const Vector &dir );

  // Returns true if this is designated as a 'caustics' map.
  bool causticOnly() const { return _causticsOnly; }
  
  // Scales photons
  void scalePower( double scale );

  // Preprocess before use
  void preprocess();

  // Store irradience estimate in irrad.
  void getIrradience( Color &irrad,
		      const Point &pos,
		      const Vector &normal,
		      double maxDistance, // To look for photons
		      int numPhotons ) const;

  inline const Photon * getPhotons(int &numPhotons) const {
    numPhotons = _storedPhotons;
    return _photons;
  }
  
private:
  Photon *_photons;
  int _storedPhotons;
  int _halfStoredPhotons;
  int _maxPhotons;
  int _prevScale;
  bool _causticsOnly;
  
  double _costheta[256];
  double _sintheta[256];
  double _cosphi[256];
  double _sinphi[256];

  BoundingBox _bounds;

  
  // Called with index = 1 to start
  void locatePhotons( NearestPhotons * const np,
		      int index ) const;

  // Store photon direction in 'direction'
  void photonDir( Vector &direction,
		  const Photon *p ) const;

  // Used for preprocessing
  void _balanceSegment( Photon **pbal, Photon **porg,
			int index, int start, int end );
  void _medianSplit( Photon **p,
		     int start, int end,
		     int median, int axis );

};
#endif
