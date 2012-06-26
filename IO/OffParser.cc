#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "IO/OffParser.h"

/*******************/
/* OFF Object Data */
/*******************/


OP_vector *Vertices;   /* array of polygon vertices */
int VertexCount;    /* number of vertices        */

/* Each polygon is an array of integer indices into the 'Vertices' */
/* array.  Polygon 'i' is stored in Polygons[i], which contains    */
/* 'EdgeCounts[i]' edges, or vertices.                             */

int **Polygons;     /* array of polygon arrays  */
int PolygonCount;   /* total number of polygons */
int *EdgeCounts;    /* number of edges (or vertices) in polygons */


/* The remaining OFF properties may or may not be specified, */ 
/* depending on  the data file.                              */


int BackFaces = OFF_UNKNOWN;      /* "back_faces" property   */
int VertexOrder = OFF_UNKNOWN;    /* "vertex_order" property */ 


/* The vertex and polygon colors may be defined only once                  */
/* as a "default" color, or they may be specified for each vertex or       */
/* polygon (but normally not both.)  In some cases, neither will be given. */
/* If a default value is given, the corrseponding "Flag" variable will be  */
/* set to true.  If they are individually specified, the corrseponding     */
/* array will not be NULL.                                                 */

int DefaultVertexColors = 0;      /* flag for "default" vertex colors        */
RGB_type DefaultVertexColor;      /* default vertex color, if flag is set    */

int DefaultPolygonColors = 0;     /* flag for "default" polygon colors       */
RGB_type DefaultPolygonColor;     /* default polygon color, if flag is set   */

RGB_type *PolygonColors = NULL;   /* array of polygon colors; if not NULL    */
                                  /* there should be 'PolygonCount' entries. */

RGB_type *VertexColors = NULL;    /* array of vertex colors; if not NULL     */
                                  /* there should be 'VertexCount' entries.  */

OP_vector *PolygonNormals = NULL;    /* array of polygon normals; if not NULL   */
                                  /* there should be 'PolygonCount' entries. */

OP_vector *VertexNormals = NULL;     /* array of polygon normals; if not NULL   */
                                  /* there should be 'VertexCount' entries.  */


/* Phong lighting characterisics may or may not be specified.         */
/* If a value is specified in the data file, the corresponding "Flag" */
/* variable will be set to true.  Otherwise it is unmodified.         */

int    DiffuseCoeffFlag = 0;      /* flag for diffuse coefficient  */
double DiffuseCoeff  = 0.5;       /* diffuse coefficient           */

int    SpecularCoeffFlag = 0;     /* flag for specular coefficient */
double SpecularCoeff = 0.35;      /* specular coefficient          */

int    SpecularExpFlag = 0;       /* flag for specular exponont    */
double SpecularExp   = 15;        /* specular exponent             */


/*****************************************************************************/

char Directory[1024];


off_property *new_property( char *name )
{
  off_property *p;

  //p = (off_property*) malloc(sizeof(off_property));
  p = new off_property;

  p->name = strdup(name);
  p->nval = 0;
  p->sval = NULL;
  p->default_data = NULL;
  p->data = NULL;

  return(p);
}

void free_property( off_property *p )
{
  int k;

  if (!p)
    return;

  if (p->sval)
    //free(p->sval);
    delete[] p->sval;
  if (p->default_data)
    //free(p->default_data);
    delete[] p->default_data;
  if (p->data) {
    for (k = 0; k < p->count; k++)
      //free(p->data[k]);
      delete[] p->data[k];
    //free(p->data);
    delete[] p->data;
  }
}

double *parse_triple( FILE *stream )
{
  double x, y, z;
  double *v;

  //v = (double*) calloc(3, sizeof(double));
  v = new double[ 3 ];

  fscanf(stream, "%lf%lf%lf", &x, &y, &z);
  v[0] = x;
  v[1] = y;
  v[2] = z;

  return(v);
}


/*****************************/
/* Annoying conversion stuff */
/*****************************/

OP_vector triple_to_vector( double *v )
{
  OP_vector p;

  p.x = v[0];
  p.y = v[1];
  p.z = v[2];

  return(p);
}

RGB_type triple_to_color( double *v )
{
  RGB_type color;

  color.R = (float) v[0];
  color.G = (float) v[1];
  color.B = (float) v[2];

  return(color);
}

void color_property( int *flag, RGB_type *default_data, RGB_type **data,
		     off_property *p )
{
  int k;
  
  if (p->default_data) {
    *flag = 1;
    *default_data = triple_to_color(p->default_data);
  }
  if (p->data) {
    *flag = 0;
    *data = (RGB_type*) calloc(p->count, sizeof(RGB_type));
    for (k = 0; k < p->count; k++)
      (*data)[k] = triple_to_color(p->data[k]);
  }    
}

void vector_property( int *flag, OP_vector *default_data, OP_vector **data,
		     off_property *p )
{
  int k;
  
  if (p->default_data) {
    *flag = 1;
    *default_data = triple_to_vector(p->default_data);
  }
  if (p->data) {
    *flag = 0;
    *data = (OP_vector*) calloc(p->count, sizeof(OP_vector));
    for (k = 0; k < p->count; k++)
      (*data)[k] = triple_to_vector(p->data[k]);
  }    
}

/************************/
/* Generic File Parsing */
/************************/


off_property *parse_generic( char *filename, char *propname )
{
  FILE *input;
  off_property *p;
  int  k, count;

  input = fopen(filename, "r");
  if (!input)
    return((off_property*) NULL);

  p = new_property(propname);

  fscanf(input, "%d", &count);
  p->count = count;
  p->data = (double**) calloc(count, sizeof(double*));
  for (k = 0; k < count; k++) {
    p->data[k] = parse_triple(input);
  }

  fclose(input);
  return(p);
}


/************************/
/* Indexed File Parsing */
/************************/

off_property *parse_indexed( char *filename, char *propname )
{
  FILE *input;
  off_property *p;
  int  k, i, count, icount;
  double **data;

  input = fopen(filename, "r");
  if (!input)
    return((off_property*) NULL);

  p = new_property(propname);
  
  fscanf(input, "%d%d", &count, &icount);
  p->count = icount;
  p->data = (double**) calloc(icount, sizeof(double*));

  data = (double**) calloc(count, sizeof(double*));
  for (k = 0; k < count; k++) {
    data[k] = parse_triple(input);
  }

  for (k = 0; k < icount; k++) {
    fscanf(input, "%d", &i);
    p->data[k] = data[i - 1];
  }
  
  fclose(input);
  return(p);
}

off_property *parse_property( FILE *stream, char *name )
{
  static char type[128];
  static char format[16];
  static char data[128];
  static char file[256];
  off_property *p = NULL;
  double value;

  fscanf(stream, "%s%s", type, format);

  if (!strcmp(type, "default")) {
    p = new_property(name);
    if (!strcmp(format, "fff")) 
      p->default_data = parse_triple(stream);
    else if (!strcmp(format, "f")) {
      fscanf(stream, "%lf", &value);
      p->nval = value;
    }
    else {
      fscanf(stream, "%s", data);
      p->sval = strdup(data);
    }
  }

  else if (!strcmp(type, "generic")) {
    fscanf(stream, "%s", data);
    sprintf(file, "%s/%s", Directory, data);
    p = parse_generic(file, name);
  }

  else if (!strcmp(type, "indexed")) {
    fscanf(stream, "%s", data);
    sprintf(file, "%s/%s", Directory, data);
    p = parse_indexed(file, name);
  }

  return(p);
}


/***********************************/
/* Geometry (indexed_poly) Parsing */
/***********************************/

void parse_geometry( char *filename )
{
  FILE *input;
  int  k, n, i;
  int  v_count, poly_count, count;
  static char file[256];
  double *v;

  sprintf(file, "%s/%s", Directory, filename);
  input = fopen(file, "r");
  if (!input)
    return;

  fscanf(input, "%d%d%d", &v_count, &poly_count, &k);

  Vertices = (OP_vector*) calloc(v_count, sizeof(OP_vector));
  VertexCount = v_count;
  PolygonCount = poly_count;
  for (k = 0; k < v_count; k++) {
    v = parse_triple(input);
    Vertices[k] = triple_to_vector(v);
    
    //free(v);
    delete v;
  }

  Polygons = (int**) calloc(poly_count, sizeof(int*));
  EdgeCounts = (int*) calloc(poly_count, sizeof(int));
  for (k = 0; k < poly_count; k++) {
    fscanf(input, "%d", &count);
    EdgeCounts[k] = count;
    Polygons[k] = (int*) calloc(count, sizeof(int));
    for (n = 0; n < count; n++) {
      fscanf(input, "%d", &i);
      Polygons[k][n] = i - 1;
    }
  }
  
  fclose(input);
}


/************************/  
/* General file parsing */
/************************/

void skip_line( FILE *stream )
{
  static char line[256];

  fgets(line, 255, stream);
}

int offPlusRead( const char *file )
{
  FILE *input;
  char  name[256];
  off_property *p=NULL;
  int blank;
  OP_vector   v;
  char     *end;

  input = fopen(file, "r");
  if (!input)
    return(0);

  strcpy(Directory, file);
  end = strrchr(Directory, '/');
  if (end)
    *end = '\0';
  else
    strcpy(Directory, ".");
    
  
  while (!feof(input)) {
    fscanf(input, "%s", name);

    if (feof(input))
      break;

    if (name[0] == '#') {
      skip_line(input);
      continue;
    }
    
    if (!strcmp(name, "geometry")) {
      /* Assume "indexed_poly" */
      fscanf(input, "%s", name);
      fscanf(input, "%s", name);
      fscanf(input, "%s", name);
      parse_geometry(name);
    }

    else if (!strcmp(name, "normals")) {
      p = parse_property(input, name);
      if (p && p->data)
	vector_property(&blank, &v, &PolygonNormals, p);
    }
    else if (!strcmp(name, "vertex_normals")) {
      p = parse_property(input, name);
      if (p && p->data)
	vector_property(&blank, &v, &VertexNormals, p);
    }
    else if (!strcmp(name, "polygon_colors")) {
      p = parse_property(input, name);
      color_property(&DefaultPolygonColors, &DefaultPolygonColor,
		   &PolygonColors, p);
    }
    else if (!strcmp(name, "vertex_colors")) {
      p = parse_property(input, name);
      color_property(&DefaultVertexColors, &DefaultVertexColor,
		     &VertexColors, p);
    }

    else if (!strcmp(name, "diffuse_coef")) {
      p = parse_property(input, name);
      DiffuseCoeff = p->nval;
      DiffuseCoeffFlag = 1;
    }
    else if (!strcmp(name, "specular_coef")) {
      p = parse_property(input, name);
      SpecularCoeff = p->nval;
      SpecularCoeffFlag = 1;
    }
    else if (!strcmp(name, "specular_power")) {
      p = parse_property(input, name);
      SpecularExp = p->nval;
      SpecularExpFlag = 1;
    }    

    else if (!strcmp(name, "vertex_order")) {
      p = parse_property(input, name);
      if (!strcmp(p->sval, "clockwise"))
	VertexOrder = CLOCKWISE_VERTICES;
      else if (!strcmp(p->sval, "counterclockwise"))
	VertexOrder = COUNTERCLOCKWISE_VERTICES;
      else if (!strcmp(p->sval, "counter-clockwise"))
	VertexOrder = COUNTERCLOCKWISE_VERTICES;
    }
    else if (!strcmp(name, "back_faces")) {
      p = parse_property(input, name);
      if (!strcmp(p->sval, "cull"))
	BackFaces = BACKFACES_CULL;
      else if (!strcmp(p->sval, "display"))
	BackFaces = BACKFACES_DISPLAY;
      else if (!strcmp(p->sval, "reverse"))
	BackFaces = BACKFACES_REVERSE;
    }
      
    else if (!strcmp(name, "name")) skip_line(input);
    else if (!strcmp(name, "description")) skip_line(input);
    else if (!strcmp(name, "copyright")) skip_line(input);
    else if (!strcmp(name, "type")) skip_line(input);
    else if (!strcmp(name, "author")) skip_line(input);

    else {
      fprintf(stderr, "Warning: unknown property: %s\n", name);
    }
  }

  free_property(p);

  return(1);
}
