#ifndef __OFF_H
#define __OFF_H

#define OFF_UNKNOWN 0


/* Possible values for the "back_faces" property */

#define BACKFACES_CULL    1
#define BACKFACES_REVERSE 2
#define BACKFACES_DISPLAY 3

/* Possible values for the "vertex_order property */

#define COUNTERCLOCKWISE_VERTICES 1
#define CLOCKWISE_VERTICES        2


/* "vector" structure, for vertices */

typedef struct {
  double x;
  double y;
  double z;
} OP_vector;


/* Structure for RGB colors */

typedef struct {
  float R;
  float G;
  float B;
} RGB_type;


typedef struct {
  char   *name;    /* OFF property name */
  double  nval;    /* numeric value*/
  char   *sval;    /* string value */

  double *default_data; /* Array holding "default" numeric data      */
  double **data;        /* Array holding "generic" or "indexed" data */
  int     count;
} off_property;


extern OP_vector *Vertices;
extern int VertexCount; 
		 
extern int **Polygons;  
extern int PolygonCount;
extern int *EdgeCounts; 

extern int BackFaces;
extern int VertexOrder;

extern int DefaultVertexColors;
extern RGB_type DefaultVertexColor;
			      
extern int DefaultPolygonColors;
extern RGB_type DefaultPolygonColor;
			      
extern RGB_type *PolygonColors;
extern RGB_type *VertexColors;
                              
extern OP_vector *PolygonNormals;
extern OP_vector *VertexNormals;

extern int    DiffuseCoeffFlag;
extern double DiffuseCoeff;

extern int    SpecularCoeffFlag;
extern double SpecularCoeff;

extern int    SpecularExpFlag;
extern double SpecularExp;


int offPlusRead( const char * );

#endif
