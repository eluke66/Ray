#include "UI/localGlut.h"

#ifdef NO_GLUT
void glutInit( int*, char ** ) {}
void glutInitDisplayMode( int ) {} 
void glutInitWindowSize( int, int ) {}
void glutInitWindowPosition( int, int ) {}
void glutCreateWindow( const char * ) {}
void glClearColor( int, int, int, double ) {}
void glViewport( int, int, int, int ) {}
void glMatrixMode( int ) {}
void glLoadIdentity() {}
void glOrtho( double, double, double,
  double, double, double ) {}
void glutDisplayFunc( void (*func)() ) {}
void glutMainLoop() {}
void glutKeyboardFunc(void (*func)(unsigned char key, int x, int y)) {}

void glColor3f( double, double, double ) {}
void glVertex2i( int, int ) {}
void glClear( int ) {}
void glBegin( int ) {}
void glEnd() {}
void glFlush() {}
void glEnable( GLenum cap ) {}
void glShadeModel( GLenum mode ) {} 
void glDrawBuffer( GLenum mode ) {} 
void glPolygonMode( GLenum face, GLenum mode ) {} 
void glReadBuffer( GLenum mode ) {} 
void glReadPixels( GLint x, GLint y,
		   GLsizei width, GLsizei height,
		   GLenum format, GLenum type,
		   GLvoid *pixels ) {} 
void glDisable( GLenum cap ) {} 
void gluPerspective (GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar) {} 
void gluLookAt (GLdouble eyeX, GLdouble eyeY, GLdouble eyeZ, GLdouble centerX, GLdouble centerY, GLdouble centerZ, GLdouble upX, GLdouble upY, GLdouble upZ) {} 
void glPushMatrix( void ) {}
void glTranslated( GLdouble x, GLdouble y, GLdouble z ) {}
void glPopMatrix( void ) {}
void glVertex3f( GLfloat x, GLfloat y, GLfloat z ) {}
void glutSolidSphere(GLdouble radius, GLint slices, GLint stacks) {}
#endif
