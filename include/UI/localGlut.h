// This file must only be included once, as it may provide function
// definitions if we don't have glut!

#include "../config.h"


#ifdef NO_GLUT
typedef double GLdouble;
typedef float GLfloat;
typedef unsigned int	GLenum;
typedef int		GLint;
typedef int		GLsizei;
typedef void		GLvoid;
void glutInit( int*, char ** ) ;
void glutInitDisplayMode( int ) ; 
void glutInitWindowSize( int, int ) ;
void glutInitWindowPosition( int, int ) ;
void glutCreateWindow( const char * ) ;
void glClearColor( int, int, int, double ) ;
void glViewport( int, int, int, int ) ;
void glMatrixMode( int ) ;
void glLoadIdentity() ;
void glOrtho( double, double, double,
  double, double, double ) ;
void glutDisplayFunc( void (*func)() ) ;
void glutMainLoop() ;
void glutKeyboardFunc(void (*func)(unsigned char key, int x, int y)) ;

void glColor3f( double, double, double ) ;
void glVertex2i( int, int ) ;
void glClear( int ) ;
void glBegin( int ) ;
void glEnd() ;
void glFlush() ;
void glEnable( GLenum cap ) ;
void glShadeModel( GLenum mode ) ; 
void glDrawBuffer( GLenum mode ) ; 
void glPolygonMode( GLenum face, GLenum mode ) ; 
void glReadBuffer( GLenum mode ) ; 
void glReadPixels( GLint x, GLint y,
		   GLsizei width, GLsizei height,
		   GLenum format, GLenum type,
		   GLvoid *pixels ) ; 
void glDisable( GLenum cap ) ; 
void gluPerspective (GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar) ; 
void gluLookAt (GLdouble eyeX, GLdouble eyeY, GLdouble eyeZ, GLdouble centerX, GLdouble centerY, GLdouble centerZ, GLdouble upX, GLdouble upY, GLdouble upZ) ; 
void glPushMatrix( void ) ;
void glTranslated( GLdouble x, GLdouble y, GLdouble z ) ;
void glPopMatrix( void ) ;
void glVertex3f( GLfloat x, GLfloat y, GLfloat z ) ;
void glutSolidSphere(GLdouble radius, GLint slices, GLint stacks) ;
#define GL_TRIANGLES 0
#define GL_POLYGON 0
#define GLUT_SINGLE 0
#define GLUT_RGB 0
#define GL_PROJECTION 0
#define GL_COLOR_BUFFER_BIT 0
#define GL_POINTS 0
#define GL_FLAT 0
#define GL_DEPTH_TEST 0
#define GL_BACK 0
#define GL_MODELVIEW 0
#define GL_DEPTH_BUFFER_BIT 0
#define GL_FRONT_AND_BACK 0
#define GL_FILL 0
#define GL_RGB 0
#define GL_UNSIGNED_BYTE 0
#define GL_FRONT 0
#define GLUT_DEPTH 0
#define GLUT_DOUBLE 0



#else // We have glut

#include <GL/glut.h>

#endif
