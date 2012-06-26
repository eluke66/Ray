#include "GlobalIllum/PhotonMap.h"
#include "GlobalIllum/Photon.h"

#include "Core/utils.h"

#include <iostream>

//#define DEBUG_MAP

// Internal class - NearestPhotons
struct NearestPhotons {
  int max;
  int found;
  bool gotHeap;
  Point pos;
  float *dist2;
  const Photon **index;
  int nPhotons;

  NearestPhotons() :
    max(0), found(0), gotHeap(false), dist2(NULL), index(NULL) {}
  NearestPhotons(int nPhotons, const Point &pos, double maxDistance) :
    max(nPhotons), found(0), gotHeap(false), pos(pos), dist2(NULL), index(NULL)
  {
    dist2 = new float[nPhotons+1];
    index = new const Photon*[nPhotons+1];
    dist2[0] = maxDistance * maxDistance;
  }
  const Photon *get(int i) {
    assert( i >= 0 && i <= max );
    assert( index );
    return index[i];
  }
  void set(int i, const Photon *p) {
    assert( i >= 0 && i <= max );
    assert( index );
    index[i] = p;
  }
  double getDist(int i) {
    assert( i >= 0 && i <= max );
    assert( index );
    return dist2[i];
  }
  void setDist(int i, double d) {
    assert( i >= 0 && i <= max );
    assert( index );
    dist2[i] = d;
  }
  double maxDist() { return dist2[0]; }
  
  ~NearestPhotons() {
    delete[] dist2;
    if ( index )
      //for ( int i = 0 ; i < max; ++i )
      //delete[] index[i];
    delete[] index;
  }
      
};

#ifdef DEBUG_MAP
void checkPhoton( Photon ** p, int start, int end )
{
  for ( int i = start; i <= end; ++i ) {
    for ( int j = i+1; j <= end; ++j )
      if ( p[i] == p[j] ) {
	std::cerr << "XXX Duplicate photon at " << i << ", " << j << std::endl;
	assert(0);
      }
  }
}
#endif

PhotonMap::PhotonMap( int maxPhotons, bool causticsOnly ) :
  _storedPhotons(0), _halfStoredPhotons(0), _maxPhotons(maxPhotons),
  _prevScale(1), _causticsOnly(causticsOnly)
{
  // Eh? Why +1?
  _photons = new Photon[ maxPhotons + 1 ];
  assert(_photons);

  for ( int i = 0; i < 256; ++i ) {
    double angle = double(i) * M_PI * 1.0/256.0;
    _costheta[i] = cos(angle);
    _sintheta[i] = sin(angle);
    _cosphi[i]   = cos(2.0*angle);
    _sinphi[i]   = sin(2.0*angle);
  }
    
}

PhotonMap::~PhotonMap()
{
  delete[] _photons;
}

// Stores a photon
void 
PhotonMap::store( const Color &power,
		  const Point &pos,
		  const Vector &dir )
{
  // Too many photons? Punt now.
  if ( _storedPhotons >= _maxPhotons )
    return;

  // Add the photon
  _storedPhotons++;
  Photon *const node = &_photons[_storedPhotons];

  node->pos( pos );
  node->color( power );
  _bounds.extend( pos );

  // Now, convert the direction
  int theta = int( acos(dir.z())*(256.0*M_PI) );
  if ( theta > 255 )
    theta = 255;

  int phi = int( atan2(dir.y(),dir.x()) * (256.0 / (2.0*M_PI)) );
  if ( phi > 255 )
    phi = 255;
  else if ( phi < 0 )
    phi = phi + 256;

  node->dir( (unsigned char)theta, (unsigned char)phi );
}

// Scales photons
void 
PhotonMap::scalePower( double scale )
{
  for ( int i = _prevScale; i <= _storedPhotons; ++i ) {
    _photons[i].color( _photons[i].color() * scale );
  }

  _prevScale = _storedPhotons + 1;
}


// Preprocess before use
void 
PhotonMap::preprocess()
{

  // Convert flat array to k-d tree
  if ( _storedPhotons > 1 ) {
    // Temp arrays
    Photon ** pa1 = new Photon*[ _storedPhotons + 1 ];
    Photon ** pa2 = new Photon*[ _storedPhotons + 1 ];
    std::cerr << "Allocating space for " << (_storedPhotons+1) << " photon pointers!" << std::endl;
    
    // Copy current array
    for ( int i = 1; i <= _storedPhotons; ++i ) {
      pa2[i] = &_photons[i];
      assert(pa2[i]);
      pa1[i] = NULL;
    }
    

#ifdef DEBUG_MAP
    // DEBUGGING
    for ( int q = 1; q <= _storedPhotons; ++q ) {
      std::cerr << "PA2["<<q<<"] = " << (void *)(pa2[q]) << " => " << pa2[q]->pos() << std::endl;
    }
#endif
    
    // Balance the whole tree
    _balanceSegment( pa1, pa2, 1, 1, _storedPhotons );

#ifdef DEBUG_MAP
    checkPhoton(pa1,1,_storedPhotons);
    checkPhoton(pa2,1,_storedPhotons);
    for ( int g = 1; g <= _storedPhotons; ++g )
      {
	bool found = false;
	for ( int l = 1; l <= _storedPhotons; ++l ) {
	  if ( pa1[l] == pa2[g] ) {
	    found = true;
	    break;
	  }
	}
	if ( !found ) {
	  std::cerr << "Photon in pa2[" <<g<<"] not found: " << pa2[g] << std::endl;
	}
	assert(found);
	
      }  
#endif
    delete[] pa2;

#ifdef DEBUG_MAP
    // DEBUGGING
    for ( int q = 1; q <= _storedPhotons; ++q ) {
      std::cerr << "PA1["<<q<<"] = " << (void *)(pa1[q]) << std::endl;
    }
#endif
    
    // Reorganized tree into a heap
    int d, j=1, tmp=1;
    Photon tmpPhoton = _photons[j];
#ifdef DEBUG_MAP
    std::cerr << "=======================================" << std::endl;
#endif
    // For each stored photon:
    for ( int i = 1; i <= _storedPhotons; ++i ) {
      // Place it in the heap

      // DAMMIT - NOT PORTABLE!
      if ( !pa1[j] ) { ERROR("Missing photon in position " << j); }
      assert(pa1[j]);
      d = pa1[j] - &_photons[0];
      assert( d <= _storedPhotons );
#ifdef DEBUG_MAP
      std::cerr << "PA1[" << j << "] = " << (void *)(pa1[j]) << std::endl;
      std::cerr << "Photons = " << (void *)(_photons) << std::endl;
      std::cerr << "D = " << d << std::endl;
#endif
      pa1[j] = NULL;

      if ( d != tmp ) {
	_photons[j] = _photons[d];
#ifdef DEBUG_MAP
	std::cerr << "Photons[" << j << "] = Photons[" << d << "]" << std::endl;
#endif
      }
      else {
	_photons[j] = tmpPhoton;

	if ( i < _storedPhotons ) {
	  for ( ; tmp <= _storedPhotons; ++tmp )
	    if ( pa1[tmp] != NULL )
	      break;
	  tmpPhoton = _photons[tmp];
	  j = tmp;
	  //std::cerr << "Setting j to " << tmp << std::endl;
	}

	continue;
      }  // Else
      j = d;
#ifdef DEBUG_MAP
      std::cerr << std::endl;
#endif
    }
    delete[] pa1;
  }

  _halfStoredPhotons = _storedPhotons/2 - 1;
}

// Store irradience estimate in irrad.
void 
PhotonMap::getIrradience( Color &irrad,
			  const Point &pos,
			  const Vector &normal,
			  double maxDistance, // To look for photons
			  int numPhotons ) const
{
  irrad = Color(0.0);

  NearestPhotons np(numPhotons, pos, maxDistance);
  

  // Locate the nearest photons.
  locatePhotons( &np, 1 );
  mdebug("Found " << np.found << " photons!" );
  
  // if less than 8 photons, there's no light. Punt.
  if ( np.found < 8 )
    return;
  
  double max_dist_2 = maxDistance * maxDistance;
  float alpha = 0.918;
  float beta = -1.953;
  float d_p_temp;
  float d_p_2;
  float r_2 = max_dist_2;
  float w_p_g;

  // surface plane - by Stephan Reiter
  const float d = Dot(pos, normal );
  const float maxdist_toplane_2 = max_dist_2 * 0.01;

  max_dist_2 = 0.0; // certain photons will be rejected, so we have
		     // to re-evaluate the max squared distance
  
  // Sum irradience from all photons.
  Vector pdir;
  for ( int i = 1; i <= np.found; ++i ) {
    const Photon *p = np.get(i);

    // Grab the photon dir, and check to see if the dot product is
    // negative??? (see that it hit the right side). Don't need this if
    // we're not using planes.
#define USING_PLANES
#ifdef USING_PLANES
    photonDir( pdir, p );
    if ( 1 || Dot( pdir, normal ) < 0.0 ) {

      ///////////////////////////////////////////////
      // NEW
#define USE_FILTER
#ifdef USE_FILTER
      
      double dist_2 = Dot(p->pos(), normal) - d;
      dist_2 *= dist_2;
      if( dist_2 > maxdist_toplane_2 ) {
	mdebug(" Skip: " << dist_2 << " > " << maxdist_toplane_2 );
	continue;
      }

      double d_p_2 = Vector(p->pos() - pos).length2();
      
      w_p_g = 1.0 - exp((beta * d_p_2) / (2.0 * r_2));
      w_p_g /= 1.0 - exp(beta);
      w_p_g = 1.0 - w_p_g;
      w_p_g *= alpha;
      mdebug(" Weight is " << w_p_g );
      
      if( d_p_2 > max_dist_2 ) {
	mdebug(" Setting max dist to " << d_p_2);
	max_dist_2 = d_p_2;
      }
      irrad += p->color() * w_p_g;
#else
      irrad += p->color();
#endif // USE_FILTER
      // NEW
      ////////////////////////////////////////////////
    }
    else {
      mdebug("Nope: " << pdir << ", norm is " << normal );
    }
#else // USING_PLANES
    mdebug("  Adding in color " << p->color() );
    irrad += p->color();
    //PRINT_TIMES(p->color(), 10 );
#endif // USING_PLANES
    
  }

  // Now, scale by the total area that we've covered. Currently, this
  // is a sphere. 1/pi * rad^2.
#ifdef USE_FILTER
  if( max_dist_2 <= 0.0 )
  {
    return;
  }
  
  double area = 1.0 / ( M_PI * max_dist_2 );
  
#else
  double area = 1.0 / ( M_PI * np.dist2[0] );
#endif
  
  mdebug("Area is " << area );
  irrad *= area; // Scale by area, and we're done.
  
  mdebug("Final color is " << irrad );
}


// Called with index = 1 to start
void 
PhotonMap::locatePhotons( NearestPhotons * const np,
			  int index ) const
{
  assert(np);
  assert( index >= 0 && index <= _maxPhotons );
  
  // Do nearest neighbors search of the k-d tree
  const Photon *p = &_photons[index];
  double dist;

  if ( index < _halfStoredPhotons ) {
    dist = np->pos.idx( p->plane() ) - p->pos().idx( p->plane() );

    // Right plane first
    if ( dist > 0.0 ) {
      locatePhotons( np, 2*index+1 );
      
      if ( dist*dist < np->maxDist() )
	locatePhotons( np, 2*index );
    }
    // Left plane first
    else {
      locatePhotons( np, 2*index );
      if ( dist*dist < np->maxDist() )
	locatePhotons( np, 2*index+1 );
    }
  }

  // Compute dist between photon and np->pos
  double dist2 = Vector(p->pos() - np->pos).length2();

  // If it's closer than the max distance, insert it!
  if ( dist2 < np->maxDist() ) {

    // If we have space, just add it to the array.
    if ( np->found < np->max ) {
      np->found++;
      np->dist2[ np->found ] = dist2;
      np->index[ np->found ] = p;
    }
    // Otherwise, use the heap
    else {
      int j,parent;
      
      // If we don't have a heap built yet, do that now.
      if ( !np->gotHeap ) {
	double dst2;
	const Photon *phot=NULL;
	int halfFound = np->found >> 1; // Quick int divide by 2.
	for ( int k=halfFound; k >=1; --k ) {
	  parent = k;
	  phot = np->get(k);
	  dst2 = np->getDist(k);

	  while ( parent <= halfFound ) {
	    j = parent+parent;
	    if ( j < np->found &&
		 np->getDist(j) < np->getDist(j+1) )
	      ++j;
	    if ( dst2 >= np->getDist(j))
		 break;
	    np->setDist(parent, np->getDist(j));
	    np->set( parent, np->get(j) );
	    parent = j;
	  } // End of while loop

	  np->setDist(parent, dst2 );
	  np->set( parent, phot );
	} // End of for loop

	np->gotHeap = true;
      } // No heap built yet

      // Insert the new photon into the heap, and re-heapify.
      parent = 1;
      j = 2;

      while ( j <= np->found ) {
	if ( j < np->found &&
	     np->getDist(j)  < np->getDist(j+1) ) 
	  ++j;
	if ( dist2 > np->getDist(j) )
	  break; // No more photons here

	np->setDist(parent, np->getDist(j));
	np->set(parent, np->get(j));
	parent = j;
	j += j;
	
      } // While loop

      if ( dist2 < np->getDist(parent) ) {
	np->set(parent, p);
	np->setDist(parent, dist2);
      }
      np->setDist( 0, np->getDist(1) );
	
    } // Use the heap

  } // Photon is close enough
  
}

void 
PhotonMap::photonDir( Vector &direction,
		      const Photon *p ) const
{
  assert(p);
  direction.set( _sintheta[p->theta()] * _cosphi[p->phi()],
		 _sintheta[p->theta()] * _sinphi[p->phi()],
		 _costheta[p->theta()] );
}


#define swap(ph,a,b) { Photon *ph2=ph[a]; ph[a] = ph[b]; ph[b] = ph2; }

#ifdef DEBUG_MAP
std::string indent = "";
#endif

// Used for preprocessing
void 
PhotonMap::_balanceSegment( Photon **pbal, Photon **porg,
			    int index, int start, int end )
{
  // Compute new median
  int median = 1;
  while ( (4*median) <= (end-start+1) )
    median += median;

  if ( (3*median) <= (end-start+1) ) {
    median += median;
    median += start - 1;
  }
  else
    median = end - median + 1;

#ifdef DEBUG_MAP
  std::cerr << indent << "Balancing Segment: " << start << " -> "
	    << median << " -> " << end << " (index is " << index << ")" << std::endl;
  indent += " ";
#endif
  assert( median <= end );
  assert( median >= start );

  // Find axis to split along
  int axis = _bounds.maxComponent();

  // Partition photons around the median
  _medianSplit( porg, start, end, median, axis );

#ifdef DEBUG_MAP  
  checkPhoton(porg,start,end);
  
  std::cerr << indent << " 1Bal["<<index<<"] = " << median << " = " << porg[median] << std::endl;
#endif
  pbal[ index ] = porg[ median ];
  pbal[ index ]->plane(axis);
  
  // Balance left and right blocks
  if ( median > start ) {
    // Left segment
    if ( start < median - 1 ) {
      BoundingBox save(_bounds);
      Point pmax = save.getMax();
      pmax.setIdx(axis, pbal[index]->pos().idx(axis) );
      _bounds.reset();
      _bounds.extend(save.getMin());
      _bounds.extend(pmax);

      _balanceSegment( pbal, porg, 2*index, start, median - 1 );
#ifdef DEBUG_MAP
      checkPhoton(porg,start,median-1);
#endif
      _bounds = save;
    }
    else {
#ifdef DEBUG_MAP
      std::cerr << indent << " 2Bal["<<2*index<<"] = " << start << " = " << porg[start] << std::endl;
#endif
      assert( 2*index <= _maxPhotons );
      pbal[ 2*index ] = porg[start];
    }
  }

  // Right segment
  if ( median < end ) {

    if ( median + 1 < end ) {
      BoundingBox save(_bounds);
      Point pmax = save.getMax();
      pmax.setIdx(axis, pbal[index]->pos().idx(axis) );
      _bounds.reset();
      _bounds.extend(save.getMin());
      _bounds.extend(pmax);
      
      _balanceSegment( pbal, porg, 2*index+1, median + 1, end );
#ifdef DEBUG_MAP
      checkPhoton(porg,median+1,end);
#endif
      _bounds = save;
    }
    else {
#ifdef DEBUG_MAP
      std::cerr << indent << " 3Bal["<<2*index+1<<"] = " << end << " = " << porg[end] << std::endl;
#endif
      assert( 2*index+1 <= _maxPhotons );
      pbal[ 2*index+1 ] = porg[end];
    }
  }
#ifdef DEBUG_MAP
  indent = indent.substr(0,indent.length()-1);
#endif
}

void 
PhotonMap::_medianSplit( Photon **p,
			 int start, int end,
			 int median, int axis )
{
  int left = start;
  int right = end;

#ifdef DEBUG_MAP
  std::cerr << indent << "MS: Axis is " << axis << std::endl;
  for ( int j = start; j < end; j++ ) {
	std::cerr << j << " -> " << p[j]->pos().idx(axis) << std::endl;
  }
#endif
  while ( right > left ) {
    assert( right < _maxPhotons );
    float v = p[right]->pos().idx(axis);
#ifdef DEBUG_MAP
    std::cerr << indent << " R=" << right << ", p=" << (void *)(p[right]) << " => " << p[right]->pos() << std::endl;
#endif
    int i = left-1;
    int j = right;

    for (;;) {
      
      while ( i < (right-1) && p[++i]->pos().idx(axis) < v ); // loop
#ifdef DEBUG_MAP
      std::cerr << indent << " V is " << v << ", and right is " << right << ", I is " << i << ", posi is " << p[i+1]->pos().idx(axis) << std::endl;
#endif
      //while ( v - p[++i]->pos().idx(axis) > 1e-5 );
      while ( p[--j]->pos().idx(axis) > v && j > left ); // loop
      if ( i >= j )
	break;
      swap(p,i,j);
    }// For
    
    swap(p,i,right);
    if ( i >= median )
      right = i - 1;
    if ( i <= median )
      left = i + 1;
  }// While


#if 0
  // Ensure that all photons above the median have greater values than
  // those below the median, and vice versa.
  double lowMaxVal = -MAXFLOAT;
  int lowMaxIdx = -1;
  for (int lower = start; lower <= median; ++lower ) {
    if ( p[lower]->pos().idx(axis) > lowMaxVal ) {
      lowMaxVal = p[lower]->pos().idx(axis);
      lowMaxIdx = lower;
    }
  }
  std::cerr << "Lower: " << start << " -> " << median-1 << std::endl;
  std::cerr << "Upper: " << median << " -> " << end-1 << std::endl;
  for ( int upper = median+1; upper < end; ++upper ) {
    if ( p[upper]->pos().idx(axis) < lowMaxVal ) {
      ERROR("Photon in position " << upper << " has a value of " << p[upper]->pos().idx(axis) << "; max value of the lower photons is " << lowMaxVal << ", found at idx " << lowMaxIdx );
      for ( int j = start; j < end; j++ ) {
	std::cerr << j << " -> " << p[j]->pos().idx(axis) << std::endl;
      }
    }
    assert(p[upper]->pos().idx(axis) >= lowMaxVal);
  }
#endif
}
