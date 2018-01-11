#include <windows.h>

#include <GL/glut.h>
#include <GL/glu.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <deque>
#include "utils/vec3.h"
#include "utils/utils.h"

#include "primitives/Figure.h"
#include "primitives/Joint.h"

/* au cas ou M_PI ne soit defini */
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define ESC 27

#define WIDTH  600
#define HEIGHT 600
float angleX=0.0f; //angle de rotation en Y de la scene
float angleY=0.0f; //angle de rotation en X de la scene
int orthoD = 4;

int selectedJoint = 0;

vec3 white( 1,1,1 );
vec3 red( 1,0,0 );
vec3 green( 0,1,0 );
vec3 blue( 0,0,1 );
vec3 black( 0,0,0 );

Figure *f1, *f2, *f3, *f4;
std::deque<Joint*> joints;

vec3 targetPoint( -4, 3, 0 );

/* initialisation d'OpenGL*/
static void init(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);

	joints.push_back( new Joint( vec3(0.,0.,0.), vec3(15,0,0) ) );
	joints.push_back( new Joint( vec3(1.,0.,0.), vec3(-40,0,0) ) );
	joints.push_back( new Joint( vec3(2.,0.,0.), vec3(-20,0,0) ) );
	joints.push_back( new Joint( vec3(3.,0.,0.), vec3(30,0,0) ) );

	f1 = new Figure( 1., joints[0], joints[1] );
	f2 = new Figure( 1., joints[1], joints[2] );
	f3 = new Figure( 1., joints[2], joints[3] );
	f4 = new Figure( 1., joints[3], NULL );
}

void drawSquare( vec3 p1, vec3 p2, vec3 p3, vec3 p4, vec3 color, GLenum mode ){
    glColor3f(color.getX(),color.getY(),color.getZ());
    glBegin( mode );
        glVertex3f( p1.getX(), p1.getY(), p1.getZ() );
        glVertex3f( p2.getX(), p2.getY(), p2.getZ() );
        glVertex3f( p3.getX(), p3.getY(), p3.getZ() );
        glVertex3f( p4.getX(), p4.getY(), p4.getZ() );
	glEnd();
}
void drawPoint( vec3 center, vec3 color ){
    glColor3f(color.getX(),color.getY(),color.getZ());
    glPointSize(3);
    glBegin( GL_POINTS );
        glVertex3f( center.getX(), center.getY(), center.getZ() );
	glEnd();
}

void drawLine( vec3 p1, vec3 p2, vec3 color ){
    glColor3f(color.getX(),color.getY(),color.getZ());
    glBegin( GL_LINES );
        glVertex3f( p1.getX(), p1.getY(), p1.getZ() );
        glVertex3f( p2.getX(), p2.getY(), p2.getZ() );
	glEnd();
}

vec3 getEndPoint( Figure * f, vec3 startPosition, vec3 startRotation ){
    return startPosition.addition( vec3( f->getLength()*cos( startRotation.getX()*PI/180. ), f->getLength()*sin( startRotation.getX()*PI/180. ),0 ) );
}

void drawIsolatedFigure( Figure * f, vec3 position, vec3 rotation, vec3 color ){
    vec3 p1 = position;
    vec3 p2 = getEndPoint( f, position, rotation );

    drawLine( p1, p2, color );
}
void drawIsolatedJoint( Joint * j, vec3 position, vec3 rotation, vec3 color ){
    vec3 p1 = position;

    drawPoint( p1, color );
}

void drawFigure( Figure * f, vec3 startPosition, vec3 startRotation, vec3 color ){
    drawIsolatedJoint( f->getStartJoint(), startPosition, startRotation, blue );

    vec3 endPosition = getEndPoint( f, startPosition, startRotation );
    drawIsolatedFigure( f, startPosition, startRotation, color );

    Joint * endJoint = f->getEndJoint();
    if( endJoint != NULL && endJoint->getOutFigure() != NULL ){
        vec3 endRotation = startRotation.addition( endJoint->getRotation() );
        drawFigure( endJoint->getOutFigure(), endPosition, endRotation, color );
    }
}

bool isTargetReached( Figure * f, vec3 startPosition, vec3 startRotation, vec3 target ){
    vec3 p = getEndPoint( f, startPosition, startRotation );
    return p.soustraction( target ).norme() < 0.05;
}

void InverseCinematic(){

}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();

    drawFigure( f1, f1->getStartJoint()->getPosition(), f1->getStartJoint()->getRotation(), red );

	glutSwapBuffers();
}

/* Au cas ou la fenetre est modifiee ou deplacee */
void reshape(int w, int h)
{
   glViewport(0, 0, (GLsizei) w, (GLsizei) h);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
        glOrtho(-orthoD, orthoD, -orthoD, orthoD, -orthoD, orthoD);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

void keyboard(unsigned char key, int x, int y)
{
   switch (key) {
    case '0': case '1': case '2': case '3':
        selectedJoint = key - '0';
        break;
    case '-':
        joints[ selectedJoint ]->rotate(Joint::Axe::X, -10);
        break;
    case '+':
        joints[ selectedJoint ]->rotate(Joint::Axe::X, 10);
        break;

   case ESC:
      exit(0);
      break;
   default :
       break;
   }

   glutPostRedisplay();
}

GLvoid manageMouse(int x, int y)
{
	angleX=x*720/WIDTH;
	angleY=-(y*180/HEIGHT-90)*4;

	//glutPostRedisplay();
}

int main(int argc, char **argv)
{
   glutInitWindowSize(WIDTH, HEIGHT);
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
   glutCreateWindow("Simulation");
   init();
   glEnable(GL_DEPTH_TEST);
   glutReshapeFunc(reshape);
   glutPassiveMotionFunc(manageMouse);
   glutKeyboardFunc(keyboard);
   glutDisplayFunc(display);
   glutMainLoop();
   return 0;
}
