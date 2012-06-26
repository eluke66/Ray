
#ifndef Colormap_h
#define Colormap_h

#include "Core/Color.h"
#include <string>
#include <vector>

class Colormap {
 public:
  Colormap(const std::string& filename);
  ~Colormap();

  void rescale(double stepsize);

  void lookup(float value, float& opacity, Color& color) const {
    int l = 0;
    int h = scaled_data.size()-1;
    if(value < scaled_data[0].value){
      opacity = scaled_data[0].opacity;
      color = scaled_data[0].color;
    } else if(value > scaled_data[h].value){
      opacity = scaled_data[h].opacity;
      color = scaled_data[h].color;
    } else {
      while(h > l+1){
        int m = (l+h)/2;
        if(value < scaled_data[m].value)
          h=m;
        else
          l=m;
      }
      float f = (value-scaled_data[l].value)/(scaled_data[l+1].value-scaled_data[l].value);
      opacity = scaled_data[l].opacity * (1-f) + scaled_data[l+1].opacity * f;
      color = scaled_data[l].color * (1-f) + scaled_data[l+1].color * f;
    }
  }
 private:
  struct Data {
    float value;
    float opacity;
    Color color;
  };
  std::vector<Data> input_data;
  std::vector<Data> scaled_data;
  int numPoints;
};

#endif
