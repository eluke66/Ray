
#include "Filter/Filters.h"
#include "Core/Image.h"
#include "Filter/Sampler.h"
#include "Core/Time.h"
#include "info.h"
#include <float.h>
#include <iomanip>
#include <iostream>
#include <string>
#include <stdlib.h>

using namespace std;

static int assignment = 5;

inline float cosfunction(float x, float y)
{
  return cos((x*x+y*y)*500*M_PI)*0.5+0.5;
}

struct GrayImageSaver {
public:
  GrayImageSaver(Image* image)
    : image(image)
  {}
  void operator()(int j, int i, float filteredvalue)
  {
    image->set(j, i, Color(filteredvalue, filteredvalue, filteredvalue));
  }
private:
  Image* image;
};

int main(int argc, char** argv)
{
  bool jittered = false;
  double t1 = Time::currentSeconds();
  if(argc != 7 && argc != 8){
    cerr << "Usage: " << argv[0] << " xres yres filter style samples outfile [filterscale]\n";
    cerr << "style:\n";
    cerr << "jn: jittered, don't reuse samples\n";
    cerr << "un: uniform, don't reuse samples\n";
    cerr << "jr: jittered, reuse samples\n";
    cerr << "ur: uniform, reuse samples\n";
    exit(1);
  }
  int xres = atoi(argv[1]);
  int yres = atoi(argv[2]);
  string filtername;
  if(*argv[3] == 'j'){
    jittered = true;
    filtername = argv[3]+1;
  } else {
    filtername = argv[3];
  }
  string style = argv[4];
  int samples = atoi(argv[5]);
  string filename = argv[6];
  float filterscale = argc==8?atof(argv[7]):1;

  Image* image = new Image(xres, yres);
  double t2 = Time::currentSeconds();

  float dx = 2./xres;
  float xmin = -1. + dx/2.;
  float dy = 2./yres;
  float ymin = -1. + dy/2.;
  Filter2D* filter = FilterFactory::findFilter2D(filtername);
  if(!filter){
    cerr << "Unknown filter: " << filtername << '\n';
    exit(1);
  }
  float filterwidth = filter->getXSupport();

  if(style == "un"){
    // I don't think this one works anymore... - Steve
    samples *= (int)filterwidth;
    float df = filterwidth/samples;
    float fmin = -filterwidth/2. + df/2.;
    float xfscale = filterscale*dx;
    float yfscale = filterscale*dy;
    int totalsamples = samples*samples;
    vector<float> weights(totalsamples);
    vector<float> xs(totalsamples);
    vector<float> ys(totalsamples);

    for(int ii=0;ii<samples;ii++){
      float fy = fmin + df*ii;
      for(int jj=0;jj<samples;jj++){
        ys[ii*samples+jj] = fy;
        float fx = fmin + df*jj;
        xs[ii*samples+jj] = fx;
      }
    }
    filter->getWeights(weights, xs, ys);
    for(int i=0;i<yres;i++){
      float px = xmin + i*dx;
      for(int j=0;j<xres;j++){
        float py = ymin + j*dy;
        float sum = 0;
        float totalweight = 0;
        float* pfw = &weights[0];
        float* pfx = &xs[0];
        float* pfy = &ys[0];
        for(int s=0;s<totalsamples;s++){
          float fy = *pfy++;
          float fx = *pfx++;

          float sx = px + fx*xfscale;
          float sy = py + fy*yfscale;
          float value = cos((sx*sx+sy*sy)*500*M_PI)*0.5+0.5;
          float weight = *pfw++;
          totalweight += weight;
          sum += weight*value;
        }
        float filteredvalue = sum/totalweight;
        image->set(j, i, Color(filteredvalue, filteredvalue, filteredvalue));
      }
    }
  } else if(style == "jn"){
    samples *= (int)filterwidth;
    float df = filterwidth/samples;
    float fmin = -filterwidth/2. + df/2.;
    float xfscale = filterscale*dx;
    float yfscale = filterscale*dy;
    int totalsamples = samples*samples;
    vector<float> weights(totalsamples);
    vector<float> xs(totalsamples);
    vector<float> ys(totalsamples);

    for(int i=0;i<yres;i++){
      float px = xmin + i*dx;
      for(int j=0;j<xres;j++){
        float py = ymin + j*dy;
        for(int ii=0;ii<samples;ii++){
          for(int jj=0;jj<samples;jj++){
            float fy = fmin + df*(ii+drand48()-0.5);
            float fx = fmin + df*(jj+drand48()-0.5);
            xs[ii*samples+jj] = fx;
            ys[ii*samples+jj] = fy;
          }
        }
        float sum = 0;
        float totalweight = 0;
        float* pfw = &weights[0];
        float* pfx = &xs[0];
        float* pfy = &ys[0];
        filter->getWeights(weights, xs, ys);
        for(int s=0;s<totalsamples;s++){
          float fy = *pfy++;
          float fx = *pfx++;

          float sx = px + fx*xfscale;
          float sy = py + fy*yfscale;
          float value = cos((sx*sx+sy*sy)*500*M_PI)*0.5+0.5;
          float weight = *pfw++;
          totalweight += weight;
          sum += weight*value;
        }
        float filteredvalue = sum/totalweight;
        image->set(j, i, Color(filteredvalue, filteredvalue, filteredvalue));
      }
    }
  } else if(style == "jr"){
    Sampler<float> sampler(samples, samples, filter,
                           xres, yres);
    sampler.sampleAndFilterRegion(0, xres, 0, yres, drand48, cosfunction, GrayImageSaver(image));
  } else {
    Sampler<float> sampler(samples, samples, filter,
                           xres, yres);
    sampler.sampleAndFilterRegion(0, xres, 0, yres, uniform, cosfunction, GrayImageSaver(image));
  }
  double t3 = Time::currentSeconds();
  image->write(filename);

  double t4 = Time::currentSeconds();
  cerr << "------------------------\n";
  cerr << "     Assignment #" << assignment << "\n";
  cerr << "------------------------\n";
  cerr << info;
  cerr << "Setup/load time:\t" << setprecision(3) << t2-t1 << " seconds\n";
  cerr << "Render time:\t\t" << setprecision(3) << t3-t2 << " seconds\n";
  cerr << "Post-process time:\t" << setprecision(3) << t4-t3 << " seconds\n";
  return 0;
}
