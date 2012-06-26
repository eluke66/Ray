#include <lib3ds/file.h>
#include <lib3ds/material.h>
#include <lib3ds/mesh.h>
#include <lib3ds/node.h>
#include <string>
#include <vector>
#include <map>

#include "Core/utils.h"
#include "IO/3DSObjectImporter.h"
#include "Material/PhongMaterial.h"
#include "Material/DielectricMaterial.h"
#include "Material/TransparentMaterial.h"
#include "Material/PhongImageMaterial.h"
#include "Material/LambertianMaterial.h"
#include "Core/Point.h"
#include "Geometry/GriddedGroup.h"
#include "IO/ImageLoader.h"
#include "Geometry/Triangle.h"
#include <libgen.h>

_3DSObjectImporter
_3DSObjectImporter::Importer;

_3DSObjectImporter::~_3DSObjectImporter() {}

Object *
_3DSObjectImporter::importFile( std::string filename ) {

  Lib3dsFile *f = NULL;
  
  f = lib3ds_file_load( filename.c_str() );
  if ( !f ) {
    ERROR("Cannot open file " << filename );
    return NULL;
  }
  lib3ds_file_eval(f,0);
  
  GriddedGroup * geom = new GriddedGroup(20);
  //Group *geom = new Group;
  {
    Lib3dsMesh *mesh;
    for (mesh = f->meshes; mesh; mesh = mesh->next ) {
      if ( !Options::Instance()->quiet() )
	std::cerr << "Mesh named " << mesh->name << std::endl;
    }
  }

  std::map<std::string,Material *> materials;
  std::string dirpath = dirname( (char *)filename.c_str() );

  getMaterials(f, dirpath, materials);
  Lib3dsMesh *mesh;
  for (mesh = f->meshes; mesh; mesh=mesh->next ) {
    if ( !Options::Instance()->quiet() )
      std::cerr << "Mesh " << " has " << mesh->faces << " faces" << std::endl;

    for (unsigned p=0; p<mesh->faces; ++p) {
      Lib3dsFace *face=&mesh->faceL[p];

      Point p1(mesh->pointL[face->points[0]].pos);
      Point p2(mesh->pointL[face->points[1]].pos);
      Point p3(mesh->pointL[face->points[2]].pos);
      if ( p1.inInterval(p2, 1e-3) || p1.inInterval(p3,1e-3) || p2.inInterval(p3,1e-3) ) {
	continue;
      }

      std::map<std::string,Material *>::iterator it = materials.find(face->material);
      if ( it == materials.end() ) {
	ERROR("Can't find material  -" << face->material << "-");
	//Triangle * tri = new Triangle( NULL, p1, p2, p3 );
	//geom->addObject(tri);
      }
      else {
	Triangle * tri = new Triangle( materials[face->material], p1, p2, p3 );
	geom->addObject(tri);
      }
    }
  }

  return geom;
}



void
_3DSObjectImporter::getMaterials(Lib3dsFile *f, std::string path, std::map<std::string,Material *> &materials ) {
  
  Material *matl = NULL;
  Lib3dsMaterial *mat=0;
  for ( mat = f->materials; mat; mat = mat->next ) {

    // Create a new material
    Vector a(mat->ambient);
    double amb = a.normalize();
    
    std::string name = path + "/" + mat->texture1_map.name;

    if ( mat->texture1_map.name[0] != 0 &&
	 ImageLoader::VerifyImage( name ) ) {
      matl = new PhongImageMaterial( name,
				     0.6,
				     amb,
				     Color( mat->specular ),
				     (int)(100*mat->shininess) );
    }
    else {

      if ( mat->transparency > 0.0 ) {

	matl = new TransparentMaterial( Color(mat->diffuse), mat->transparency );
      }
      else {
#if 0
	matl = new PhongMaterial( Color( mat->diffuse ),
				  0.6f,
				  amb,
				  Color( mat->specular ),
				  (int)(100*mat->shininess) );
#else
	matl = new LambertianMaterial( mat->diffuse, 0.6f, amb );
#endif
      }
    }
    
    if ( ! Options::Instance()->quiet() ) {
      std::cerr << "Adding material " << mat->name << std::endl;
    }
    materials[ mat->name ] = matl;
  }
}
