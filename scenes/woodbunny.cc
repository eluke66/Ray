#include "all.h"
#include <fstream>
#include <iostream>
#include <vector>
using std::cerr;
using std::ifstream;
using std::vector;

Scene* woodbunny()
{
  Scene* scene = new Scene();
  scene->setBackground(new ConstantBackground(Color(0.5, 0.8, 0.9)));
  Group* world = new Group();
  GriddedGroup* bunny = new GriddedGroup(20);
  world->addObject(bunny);

  Material* marble = new PhongMarbleMaterial(Color(0.1, 0.2, 0.5), // Color 1
                                             Color(0.7, 0.8, 1.0), // Color 2
                                             10, // scale
                                             8, // octaves
                                             20, // turbulence scale
                                             1, // frequency scale
                                             2, // lacunarity
                                             0.6, // gain
                                             0.6, 0.4, Color(1,1,1), 120);
  world->addObject(new Box(marble,
                           Point(-0.1, 0, -0.1), Point(0.1, 0.032, 0.1)));

  Material* bunnymatl = new PhongWoodMaterial(Color(227, 210, 187)/255.,
                                              Color(181, 160, 117)/255.,
                                             300, // scale
                                             2, // octaves
                                             5, .04, // ring noise scale, frequency
                                             1, .1, // center noise scale, frequency
                                             2, // lacunarity
                                             0.6, // gain
                                             10, // expon
                                              0.5, 0.5, Color(0,0,0), 120);
  ifstream in("data/bunny.tm");
  if(!in)
    error();
  bool have_normals, have_something_else;
  in >> have_normals >> have_something_else;
  int numverts;
  in >> numverts;
  if(!in)
    error();
  cerr << "reading " << numverts << " vertices...";
  vector<Point> verts(numverts);
  for(int i=0;i<numverts;i++){
    double x,y,z;
    in >> x >> y >> z;
    if(!in)
      error();
    verts[i] = Point(x,y,z);
  }
  int numtris;
  in >> numtris;
  cerr << " reading " << numtris << " triangles...";
  for(int i=0;i<numtris;i++){
    int i1, i2, i3;
    in >> i1 >> i2 >> i3;
    if(!in)
      error();
    bunny->addObject(new Triangle(bunnymatl, verts[i1], verts[i2], verts[i3]));
  }
  cerr << " done\n";
    
  scene->setObject(world);

  scene->setAmbient(Color(.4, .4, .4));
  scene->addLight(new PointLight(Point(20, 300, 10), Color(1,1,1)));

  scene->setCamera(new PinholeCamera(Point(-.7,.5,.7),
                                     Point(-0.02, .10, -0.02),
                                     Vector(0, 1, 0),
                                     10));
  scene->setMaxRayDepth(25);
  scene->setMinAttenuation(.01);
  scene->setSampling(Scene::Jittered, 4);

  return scene;
}

ADD_SCENE(woodbunny)
