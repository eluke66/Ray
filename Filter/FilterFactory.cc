
#include "Filter/FilterFactory.h"
#include "Filter/Filters.h"
#include <string>
#include <iostream>
using namespace std;


const char * filters[] = {"Box", "Triangle", "LanczosSinc1", "LanczosSinc2", "LanczosSinc3",
			  "TruncatedGaussian2", "TruncatedGaussian2" };
const char * BEST_FILTER = "LanczosSinc3";
const int NUM_FILTERS = sizeof(filters) / sizeof(const char *);


// This could be improved massively...
void
FilterFactory::PrintFilterTypes() {
  int i;

  for ( i = 0; i < NUM_FILTERS; ++i )
    cerr << filters[i] << " ";
  cerr << "best ('" << BEST_FILTER << "') ";
  
  for ( i = 0; i < NUM_FILTERS; ++i )
    cerr << "r" << filters[i] << " ";
  cerr << "rbest ('r" << BEST_FILTER << "') ";
 
  cerr << endl;
}

Filter1D* FilterFactory::findFilter1D(const std::string& filterspec_in)
{

  std::string filterspec = filterspec_in;
  if ( filterspec == "best" )
    filterspec = BEST_FILTER;
  
  if(filterspec == "Box")
    return new Filter1DWrapper<BoxKernel1D>;
  else if(filterspec == "Triangle")
    return new Filter1DWrapper<TriangleKernel1D>;
  else if(filterspec == "LanczosSinc1")
    return new Filter1DWrapper<LanczosWindow1D<SincKernel1D> >(LanczosWindow1D<SincKernel1D>(1.0));
  else if(filterspec == "LanczosSinc2")
    return new Filter1DWrapper<LanczosWindow1D<SincKernel1D> >(LanczosWindow1D<SincKernel1D>(2.0));
  else if(filterspec == "LanczosSinc3")
    return new Filter1DWrapper<LanczosWindow1D<SincKernel1D> >(LanczosWindow1D<SincKernel1D>(3.0));
  else if(filterspec == "TruncatedGaussian2")
    return new Filter1DWrapper<BoxWindow1D<GaussianKernel1D> >(BoxWindow1D<GaussianKernel1D>(2.0));
  else if(filterspec == "TruncatedGaussian3")
    return new Filter1DWrapper<BoxWindow1D<GaussianKernel1D> >(BoxWindow1D<GaussianKernel1D>(3.0));
  else
    return 0;
}

Filter2D* FilterFactory::findFilter2D(const std::string& fspec)
{
  if(fspec[0] == 'r'){
    string filterspec = fspec.substr(1, fspec.length()-1);
    if ( filterspec == "best" )
      filterspec = BEST_FILTER;
    
    if(filterspec == "Box")
      return new Filter2DWrapper<RadialKernel2D<BoxKernel1D> >;
    else if(filterspec == "Triangle")
      return new Filter2DWrapper<RadialKernel2D<TriangleKernel1D> >;
    else if(filterspec == "LanczosSinc1")
      return new Filter2DWrapper<RadialKernel2D<LanczosWindow1D<SincKernel1D> > >(LanczosWindow1D<SincKernel1D>(1.0));
    else if(filterspec == "LanczosSinc2")
      return new Filter2DWrapper<RadialKernel2D<LanczosWindow1D<SincKernel1D> > >(LanczosWindow1D<SincKernel1D>(2.0));
    else if(filterspec == "LanczosSinc3")
      return new Filter2DWrapper<RadialKernel2D<LanczosWindow1D<SincKernel1D> > >(LanczosWindow1D<SincKernel1D>(3.0));
    else if(filterspec == "TruncatedGaussian2")
      return new Filter2DWrapper<RadialKernel2D<BoxWindow1D<GaussianKernel1D> > >(BoxWindow1D<GaussianKernel1D>(2.0));
    else if(filterspec == "TruncatedGaussian3")
      return new Filter2DWrapper<RadialKernel2D<BoxWindow1D<GaussianKernel1D> > >(BoxWindow1D<GaussianKernel1D>(3.0));
    else
      return 0;
  } else {
    string filterspec = fspec;
    if ( filterspec == "best" )
      filterspec = BEST_FILTER;
    
    if(filterspec == "Box")
      return new Filter2DWrapper<SeparableKernel2D<BoxKernel1D> >;
    else if(filterspec == "Triangle")
      return new Filter2DWrapper<SeparableKernel2D<TriangleKernel1D> >;
    else if(filterspec == "LanczosSinc1")
      return new Filter2DWrapper<SeparableKernel2D<LanczosWindow1D<SincKernel1D> > >(LanczosWindow1D<SincKernel1D>(1.0));
    else if(filterspec == "LanczosSinc2")
      return new Filter2DWrapper<SeparableKernel2D<LanczosWindow1D<SincKernel1D> > >(LanczosWindow1D<SincKernel1D>(2.0));
    else if(filterspec == "LanczosSinc3")
      return new Filter2DWrapper<SeparableKernel2D<LanczosWindow1D<SincKernel1D> > >(LanczosWindow1D<SincKernel1D>(3.0));
    else if(filterspec == "TruncatedGaussian2")
      return new Filter2DWrapper<SeparableKernel2D<BoxWindow1D<GaussianKernel1D> > >(BoxWindow1D<GaussianKernel1D>(2.0));
    else if(filterspec == "TruncatedGaussian3")
      return new Filter2DWrapper<SeparableKernel2D<BoxWindow1D<GaussianKernel1D> > >(BoxWindow1D<GaussianKernel1D>(3.0));
    else
      return 0;
  }
}
