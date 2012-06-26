
#include "Core/Image.h"
#include "Core/PerlinNoise.h"
#include "Core/Point.h"
#include "Core/Time.h"
#include <float.h>
#include <iomanip>
#include <iostream>
#include <string>
using namespace std;

int main(int argc, char** argv)
{
  if(argc < 6){
    cerr << "Usage: " << argv[0] << " filename xres yres scale type [options]\n";
    exit(1);
  } 
  string filename = argv[1];
  int xres = atoi(argv[2]);
  int yres = atoi(argv[3]);
  double scale = atof(argv[4]);
  string type = argv[5];

  Image* image = new Image(xres, yres);
  double t2 = Time::currentSeconds();

  float dx = 2./xres*scale;
  float xmin = -scale + dx/2.;
  float dy = 2./yres*scale;
  float ymin = -scale + dy/2.;

  if(type == "marble"){
    if(argc != 12){
      cerr << "marble options: octaves tscale fscale lacunarity gain\n";
      exit(1);
    }
    int octaves = atoi(argv[6]);
    double tscale = atof(argv[7]);
    double fscale = atof(argv[8]);
    double lacunarity = atof(argv[9]);
    double gain = atof(argv[10]);
    int expon = atoi(argv[11]);
    for(int i=0;i<yres;i++){
      float py = xmin + i*dx;
      for(int j=0;j<xres;j++){
        float px = ymin + j*dy;
        double value = cos(px+tscale*turbulence(octaves, Point(px*fscale, py*fscale, 0),
                                                lacunarity, gain));
        double svalue = value*0.5+0.5;
        svalue = 1-ipow(svalue, expon);
        image->set(j, i, Color(svalue, svalue, svalue));
      }
    }
  } else if(type == "marbleabs"){
    if(argc != 11){
      cerr << "marble options: octaves tscale fscale lacunarity gain\n";
      exit(1);
    }
    int octaves = atoi(argv[6]);
    double tscale = atof(argv[7]);
    double fscale = atof(argv[8]);
    double lacunarity = atof(argv[9]);
    double gain = atof(argv[10]);
    for(int i=0;i<yres;i++){
      float px = xmin + i*dx;
      for(int j=0;j<xres;j++){
        float py = ymin + j*dy;
        double value = cos(px+tscale*turbulenceAbs(octaves, Point(px*fscale, py*fscale, 0),
                                                lacunarity, gain));
        double svalue = value*0.5+0.5;
        image->set(j, i, Color(svalue, svalue, svalue));
      }
    }
  } else if(type == "crosscutwood" || type == "plainsawnwood"){
    if(argc != 11){
      cerr << "marble options: expon\n";
      exit(1);
    }
    int expon = atoi(argv[6]);
    double rscale = atof(argv[7]);
    double rfreq = atof(argv[8]);
    double cscale = atof(argv[9]);
    double cfreq = atof(argv[10]);
    //int octaves = atoi(argv[6]);
    //double tscale = atof(argv[7]);
    //double fscale = atof(argv[8]);
    //double lacunarity = atof(argv[9]);
    //double gain = atof(argv[10]);
    Color c1(.71, .71, .71);
    Color c2(0.62, 0.58, 0.55);
    Vector xaxis, yaxis, zaxis;
    double zoff;
    double rxoff, ryoff;
    if(type == "crosscutwood"){
      xaxis = Vector(1,0,0);
      yaxis = Vector(0,1,0);
      zaxis = Vector(0,0,1);
      zoff = 0;
      rxoff = ryoff = 0;
    } else {
      xaxis = Vector(1,0,0);
      yaxis = Vector(0,.04,1);
      zaxis = Vector(0,1,0);
      zoff = 1;
      rxoff = 0;
      ryoff = 10;
    }
    for(int i=0;i<yres;i++){
      float pi = xmin + i*dx;
      for(int j=0;j<xres;j++){
        float pj = ymin + j*dy;
        double px = Dot(xaxis, Vector(pi, pj, zoff))-rxoff;
        double py = Dot(yaxis, Vector(pi, pj, zoff))-ryoff;
        double pz = Dot(zaxis, Vector(pi, pj, zoff));
        double cx = cscale*perlinNoise(Point(px*cfreq, py*cfreq, pz*cfreq));
        double cy = cscale*perlinNoise(Point(px*cfreq+1, py*cfreq+1, pz*cfreq+1));
        //cerr << "cx=" << cx << ", cy=" << cy << '\n';
        px -= cx;
        py -= cy;
        float r = sqrt(px*px+py*py)+rscale*perlinNoise(Point(px*rfreq, py*rfreq, pz*rfreq));
        double value = cos(r)*0.5+0.5;
        value = ipow(value, expon);
        Color wood = c1*(1-value)+c2*value;
        image->set(j, i, wood);
      }
    }
  } else {
    cerr << "Unknown texture type: " << type << '\n';
    exit(1);
  }
  double t3 = Time::currentSeconds();
  image->write(filename);
  cerr << "Render time:\t\t" << setprecision(3) << t3-t2 << " seconds\n";
  return 0;
}
