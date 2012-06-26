#include "all.h"
#include <fstream>
#include <iostream>
#include <vector>
using std::cerr;
using std::ifstream;
using std::vector;

Scene* scene6()
{
  Scene* scene = new Scene();
  scene->setBackground(new ConstantBackground(Color(0.5, 0.8, 0.9)));
  Group* world = new Group();
  GriddedGroup* bunny = new GriddedGroup(10);
  world->addObject(bunny);

  Material* pedestalmatl = new LambertianMaterial(Color(0.9, 0.9, 0.9), .6, .4);
  world->addObject(new Box(pedestalmatl,
                           Point(-0.1, 0, -0.1), Point(0.1, 0.032, 0.1)));

  Material* bunnymatl = new PhongMaterial(Color(0.6, 0.4, .3), 0.6, 0.4,
                                          Color(1,1,1), 80);

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
  scene->addLight(new PointLight(Point(20, 30, 100), Color(.9,.9,.9)));
  scene->addLight(new PointLight(Point(-40, -30, 50), Color(.3,.1,.1)));

  scene->setCamera(new PinholeCamera(Point(-.7,.5,.7),
                                     Point(-0.02, .10, -0.02),
                                     Vector(0, 1, 0),
                                     10));
  scene->setMaxRayDepth(25);
  scene->setMinAttenuation(.01);
  //scene->setNumSamples(5);

  return scene;
}

ADD_SCENE(scene6)
