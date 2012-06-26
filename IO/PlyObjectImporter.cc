#include "IO/PlyObjectImporter.h"
#include "Core/utils.h"
#include "IO/ply.h"
#include "Geometry/Object.h"
#include "Core/Options.h"
#include "Geometry/GriddedGroup.h"
#include "Geometry/Triangle.h"
#include "Core/Point.h"
#include "string.h"

PlyObjectImporter
PlyObjectImporter::Importer;

typedef struct MyVertex {
  float x,y,z;             /* the usual 3-space position of a vertex */
  float intensity;
} MyVertex;

typedef struct Face {
  unsigned char intensity; /* this user attaches intensity to faces */
  unsigned char nverts;    /* number of vertex indices in list */
  int *verts;              /* vertex index list */
} Face;

PlyProperty vert_props[] = { /* list of property information for a vertex */
  {"x", PLY_FLOAT, PLY_FLOAT, offsetof(MyVertex,x), 0, 0, 0, 0},
  {"y", PLY_FLOAT, PLY_FLOAT, offsetof(MyVertex,y), 0, 0, 0, 0},
  {"z", PLY_FLOAT, PLY_FLOAT, offsetof(MyVertex,z), 0, 0, 0, 0},
  {"intensity", PLY_FLOAT, PLY_FLOAT, offsetof(MyVertex,intensity),
   0, 0, 0, 0}
    
};

PlyProperty face_props[] = { /* list of property information for a vertex */
    {"intensity", PLY_FLOAT, PLY_FLOAT, offsetof(Face,intensity), 0, 0, 0, 0},
    {"vertex_indices", PLY_INT, PLY_INT, offsetof(Face,verts),
     1, PLY_UCHAR, PLY_UCHAR, offsetof(Face,nverts)},
};
int PRINT = 0;
Object *
PlyObjectImporter::importFile( std::string filename ) {

  int i,j;
  PlyFile *ply = NULL;
  int nelems;
  char **elist;
  int file_type;
  float version;
  int nprops;
  int num_elems;
  PlyProperty **plist;
  MyVertex **vlist=NULL;
  Face **flist;
  char *elem_name;
  unsigned int numVertices = 0;
  
  char * name = strdup( filename.c_str() );
  ply = ply_open_for_reading( name,
			      &nelems,
			      &elist,
			      &file_type,
			      &version );
  assert( ply );
  free(name);

  GriddedGroup *geom = new GriddedGroup(20);
  
  for (i = 0; i < nelems; i++) {
    elem_name = elist[i];
    plist = ply_get_element_description (ply, elem_name, &num_elems, &nprops);

    /* if we're on vertex elements, read them in */
    if (equal_strings ("vertex", elem_name)) {

      /* create a vertex list to hold all the vertices */
      vlist = (MyVertex **) malloc (sizeof (MyVertex *) * num_elems);

      /* set up for getting vertex elements */

      ply_get_property (ply, elem_name, &vert_props[0]);
      ply_get_property (ply, elem_name, &vert_props[1]);
      ply_get_property (ply, elem_name, &vert_props[2]);

      /* grab all the vertex elements */
      for (j = 0; j < num_elems; j++) {
        /* grab and element from the file */
        vlist[j] = (MyVertex *) malloc (sizeof (MyVertex));
        ply_get_element (ply, (void *) vlist[j]);

#ifdef VERY_DEBUG
	std::cerr << "Vertex " << j << " is: " <<
	  vlist[j]->x << ", " << vlist[j]->y <<
	  ", " << vlist[j]->z << std::endl;
#endif
      }

      numVertices = num_elems;
    }

    /* if we're on face elements, read them in */
    if (equal_strings ("face", elem_name)) {

      if (!Options::Instance()->quiet() )
	std::cerr << "PLY has " << num_elems << " polygons..." << std::endl;

      /* create a list to hold all the face elements */
      flist = (Face **) malloc (sizeof (Face *) * num_elems);

      /* set up for getting face elements */

      ply_get_property (ply, elem_name, &face_props[0]);
      ply_get_property (ply, elem_name, &face_props[1]);

      /* grab all the face elements */
      for (j = 0; j < num_elems; j++) {

#ifdef DEBUGGING
	if ( !(j % 500) && !g_quietMode )
	  std::cerr << "." << std::flush;
#endif
        /* grab and element from the file */
        flist[j] = (Face *) malloc (sizeof (Face));
        ply_get_element (ply, (void *) flist[j]);

	// Triangles only right now, please!
	float scale = 1.0f;
	if ( flist[j]->nverts == 3 ) {
	  assert( flist[j]->nverts == 3 );
	  MyVertex *mv = vlist[flist[j]->verts[0]];
	  Point p1( mv->x / scale, mv->y / scale, mv->z / scale );
	  mv = vlist[flist[j]->verts[1]];
	  Point p2(  mv->x / scale, mv->y / scale, mv->z / scale );
	  mv = vlist[flist[j]->verts[2]];
	  Point p3(  mv->x / scale, mv->y / scale, mv->z / scale );
	  Triangle * triangle = new Triangle( NULL,
					      p1, p2, p3 );
	
	  geom->addObject( triangle );
	}
	else {
	  std::cerr << "Has " << (int)flist[j]->nverts << " vertices" << std::endl;
	}
	
      }
    }
  }
  
  /* close the PLY file */
  ply_close (ply);

  return geom;
  
}

