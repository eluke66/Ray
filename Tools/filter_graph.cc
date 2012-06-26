
#include "Filter/Filters.h"
#include <fstream>
#include <iostream>
#include <string>
#include <stdlib.h>
using namespace std;

static char* filters[] = {"Box", "Triangle",
                          "LanczosSinc1", "LanczosSinc2", "LanczosSinc3",
                          "TruncatedGaussian2", "TruncatedGaussian3"};

int main(int argc, char** argv)
{
  int samples = 101;
  int numfilters = sizeof(filters)/sizeof(char*);
  ofstream out("filters.dat");
  vector<float> xs(samples);
  vector<float> weights(samples);
  for(int i=0;i<numfilters;i++){
    Filter1D* filter = FilterFactory::findFilter1D(filters[i]);
    if(!filter){
      cerr << "Cannot find filter: " << filters[i] << '\n';
      exit(1);
    }
    float supp = 8;

    if(i != 0)
      out << "&\n";

    // Set up x samples
    for(int j=0;j<samples;j++){
      float x = -supp/2 + supp*j/(samples-1);
      xs[j] = x;
    }
    filter->getWeights(weights, xs);
    // graph x samples
    for(int j=0;j<samples;j++)
      out << xs[j] << " " << weights[j] << '\n';
  }
}
