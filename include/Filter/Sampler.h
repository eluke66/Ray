
#ifndef Sampler_h
#define Sampler_h

#include <vector>

template<class Data>
class Sampler {
public:
  Sampler(int xsamples, int ysamples, Filter2D* filter,
          int xres, int yres)
    : xsamples(xsamples), ysamples(ysamples), filter(filter)
  {
    double xextra = filter->getXSupport()/2-0.5;
    if(xextra > 1.e-6)
      xghost = (int)(xextra+0.999999);
    else
      xghost = 0;
    double yextra = filter->getYSupport()/2-0.5;
    if(yextra > 1.e-6)
      yghost = (int)(yextra+0.999999);
    else
      yghost = 0;
 
    xsupp = 2*xghost+1;
    ysupp = 2*yghost+1;
    totalsamples = xsamples*ysamples;
    totalsupp = xsupp*ysupp;
    weights.resize(totalsupp*totalsamples);
    xs.resize(totalsupp*totalsamples);
    ys.resize(totalsupp*totalsamples);
    data.resize(totalsupp*totalsamples);
    dx = 2./xres;
    dy = 2./yres;
    dfx = 1./xsamples;
    dfy = 1./ysamples;
  }

  int xsamples, ysamples;
  Filter2D* filter;
  int xghost, yghost;
  int xsupp, ysupp;
  int totalsupp, totalsamples;
  std::vector<float> weights;
  std::vector<float> xs;
  std::vector<float> ys;
  std::vector<Data> data;
  float dx, dy;
  float dfx, dfy;
  struct Sample {
    float x, y;
    Data data;
  };


  template<class Jitter, class Function, class Saver>
  void sampleAndFilterRegion(int xstart, int xend, int ystart, int yend,
                             Jitter jitter, Function function, Saver saver)
  {
    Sample* samps = new Sample[ysupp*((xend-xstart)+2*xghost)*totalsamples];
    Sample** samplerows = new Sample*[ysupp];
    Sample** del_samplerows = samplerows;
    for(int i=0;i<ysupp;i++)
      samplerows[i] = samps+i*((xend-xstart)+2*xghost)*totalsamples;

    for(int i=ystart-2*yghost;i<yend;i++){
      int si=i+yghost;
      float py = si*dy-1;
      Sample* sample = samplerows[ysupp-1];
      for(int sj=xstart-xghost;sj<xend+xghost;sj++){
        float px = sj*dx-1;
        Sample* samp = &sample[(sj-xstart+xghost)*totalsamples];
        for(int ii=0;ii<ysamples;ii++){
          for(int jj=0;jj<xsamples;jj++){
            float sy = (ii+jitter())*dfy;
            float sx = (jj+jitter())*dfx;
            samp->x = sx-0.5;
            samp->y = sy-0.5;
            float x = px+sx*dx;
            float y = py+sy*dy;
            samp->data = function(x,y);
            samp++;
          }
        }
      }
      if(i >= 0){
        // Filter and output
        for(int j=xstart;j<xend;j++){
          float* pxs = &xs[0];
          float* pys = &ys[0];
          Data* pdata = &data[0];
          for(int ii=-yghost;ii<=yghost;ii++){
            Sample* samp = &samplerows[ii+yghost][(j-xstart)*totalsamples];
            for(int jj=-xghost;jj<=xghost;jj++){
              for(int s=0;s<totalsamples;s++){
                *pxs++ = samp->x+jj;
                *pys++ = samp->y+ii;
                *pdata++ = samp->data;
                samp++;
              }
            }
          }
          filter->getWeights(weights, xs, ys);
          int total = totalsupp*totalsamples;
          float totalweight = weights[0];
          Data sum = data[0]*weights[0];
          for(int s=1;s<total;s++){
            sum += data[s]*weights[s];
            totalweight += weights[s];
          }
          Data filteredvalue = sum/totalweight;
          saver(j, i, filteredvalue);
        }
      }

      // Rotate window down
      Sample* s = samplerows[0];
      for(int i=0;i<ysupp-1;i++)
        samplerows[i] = samplerows[i+1];
      samplerows[ysupp-1] = s;
    }
    delete[] samps;
    delete[] del_samplerows;
  }
};

inline float uniform() { return 0.5; }

#endif
