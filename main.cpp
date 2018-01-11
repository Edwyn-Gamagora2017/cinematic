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
float mouseX=0.0f;
float mouseY=0.0f;
int orthoD = 4;

int selectedJoint = 0;

vec3 white( 1,1,1 );
vec3 red( 1,0,0 );
vec3 green( 0,1,0 );
vec3 blue( 0,0,1 );
vec3 black( 0,0,0 );

Figure *f1, *f2, *f3, *f4;
std::deque<Joint*> joints;

vec3 startP( 0, 0, 0 );
vec3 targetP( -3, 3, 0 );

/* initialisation d'OpenGL*/
static void init(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);

	joints.push_back( new Joint( startP, vec3(15,0,0) ) );
	joints.push_back( new Joint( vec3(1.,0.,0.), vec3(-40,0,0) ) );
	joints.push_back( new Joint( vec3(2.,0.,0.), vec3(-20,0,0) ) );
	joints.push_back( new Joint( vec3(3.,0.,0.), vec3(30,0,0) ) );
	joints.push_back( new Joint( vec3(0.,0.,0.), vec3(0,0,0) ) );

	f1 = new Figure( 1.2, joints[0], joints[1] );
	f2 = new Figure( .3, joints[1], joints[2] );
	f3 = new Figure( .8, joints[2], joints[3] );
	f4 = new Figure( 1.7, joints[3], joints[4] );
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

void drawIsolatedFigure( Figure * f, vec3 color ){
    vec3 p1 = f->getStartJoint()->getRelPosition();
    vec3 p2 = f->getEndJoint()->getRelPosition();

    drawLine( p1, p2, color );
}
void drawIsolatedJoint( Joint * j, vec3 color ){
    vec3 p1 = j->getRelPosition();

    drawPoint( p1, color );
}
void draw( Joint * j, vec3 color ){
    drawIsolatedJoint( j, blue );
    if( j->getOutFigure() != NULL ){
        drawIsolatedFigure( j->getOutFigure(), color );
    }

    if( j->getOutFigure() != NULL && j->getOutFigure()->getEndJoint() != NULL ){
        draw( j->getOutFigure()->getEndJoint(), color );
    }
}

void calculateJoint( Joint * j, vec3 startPosition, vec3 startRotation ){
    j->setRelPosition( startPosition );
    j->setRelRotation( startRotation );

    if( j->getOutFigure() != NULL && j->getOutFigure()->getEndJoint() != NULL ){
        vec3 endPosition = getEndPoint( j->getOutFigure(), startPosition, startRotation );
        vec3 endRotation = startRotation.addition( j->getOutFigure()->getEndJoint()->getRotation() );
        calculateJoint( j->getOutFigure()->getEndJoint(), endPosition, endRotation );
    }
}

bool isTargetReached( Joint * j, vec3 target ){
    return j->getRelPosition().soustraction( target ).norme() < 0.05;
}

void InverseCinematic( Joint * joint, vec3 targetPosition, bool fromBegin ){
    joint->setRelPosition( targetPosition );

    Figure * figure = (fromBegin?joint->getOutFigure():joint->getInFigure());
    if( figure != NULL ){
        Joint * otherJoint = fromBegin?figure->getEndJoint():figure->getStartJoint();
        if( otherJoint != NULL ){
            vec3 vecNewTarget = otherJoint->getRelPosition().soustraction( targetPosition ).normalized().multiplication( figure->getLength() );
            vec3 newTarget = targetPosition.addition( vecNewTarget );

            // Update Original Angle
            /*vec3 oldRot = joint->getRotation();
            joint->setRotation( vec3( acos( (newTarget.getX()-targetPosition.getX())/figure->getLength() )*180/PI, oldRot.getY(), oldRot.getZ() ) );*/

            InverseCinematic( otherJoint, newTarget, fromBegin );
        }
    }
}

void UpdateRotations( std::deque<Joint *> j ){
    vec3 accRotation(0,0,0);
    for( int i=0; i<joints.size(); i++ ){
        Joint * joint = j[i];
        Figure * figure = joint->getOutFigure();
        if( figure != NULL ){
            Joint * otherJoint = figure->getEndJoint();
            if( otherJoint != NULL ){
                vec3 targetPosition = joint->getRelPosition();
                vec3 vecNewTarget = otherJoint->getRelPosition().soustraction( targetPosition ).normalized().multiplication( figure->getLength() );
                vec3 newTarget = targetPosition.addition( vecNewTarget );

                // Update Original Angle
                vec3 oldRot = joint->getRotation();
                float distX = (newTarget.getX()-targetPosition.getX());
                float distY = (newTarget.getY()-targetPosition.getY());
                vec3 newRot = vec3( (distY>0?1:-1)*acos( distX/figure->getLength() )*180/PI, oldRot.getY(), oldRot.getZ() );

                joint->setRotation( newRot.soustraction( accRotation ) );
                accRotation = newRot;
            }
        }
    }
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();

	drawPoint( targetP, green );
	drawPoint( startP, green );

    /*calculateJoint( joints[0], joints[0]->getPosition(), joints[0]->getRotation() );
    draw( joints[0], white );*/

    int cinemIT = 0;
    while( cinemIT < 10 && !( isTargetReached( joints[joints.size()-1], targetP ) ) ){
        InverseCinematic( joints[joints.size()-1],targetP,false );
        InverseCinematic( joints[0],startP,true );
        cinemIT++;
    }
    UpdateRotations(joints);
    draw( joints[0], red );

    calculateJoint( joints[0], joints[0]->getPosition(), joints[0]->getRotation() );
    draw( joints[0], white );

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
	mouseX=x/(float)WIDTH;
	mouseY=1.-y/(float)HEIGHT;

	targetP = vec3( -orthoD+(2*mouseX*orthoD), -orthoD+(2*mouseY*orthoD), 0 );

	glutPostRedisplay();
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
