#include <windows.h>

#include <GL/glut.h>
#include <GL/glu.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <deque>
#include "utils/vec3.h"
#include "utils/utils.h"

#include <ctime>
#include <iostream>
#include <cstdlib>

#include "primitives/Figure.h"
#include "primitives/Joint.h"
#include "primitives/Arm.h"

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
vec3 yellow( 1,1,0 );

// TODO target em cima de um ponto : normalizacao de vetor nulo

std::deque<Joint*> _joints;
std::deque<Figure*> _figures;
Arm * _arm;
int nSolutions = 5;

vec3 startP( 0, 0, 0 );
vec3 targetP( -5, 1, 0 );

/* initialisation d'OpenGL*/
static void init(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);

	_joints.push_back( new Joint( startP, vec3(15,0,0), 90 ) );
	_joints.push_back( new Joint( vec3(1.,0.,0.), vec3(-40,0,0), 20 ) );
	_joints.push_back( new Joint( vec3(2.,0.,0.), vec3(-20,0,0), 30 ) );
	_joints.push_back( new Joint( vec3(3.,0.,0.), vec3(30,0,0), 40 ) );
	_joints.push_back( new Joint( vec3(3.,0.,0.), vec3(30,0,0), 40 ) );
	_joints.push_back( new Joint( vec3(3.,0.,0.), vec3(30,0,0), 40 ) );
	_joints.push_back( new Joint( vec3(4.,0.,0.), vec3(0,0,0) ) );

	_figures.push_back( new Figure( 1.2, _joints[0], _joints[1] ) );
	_figures.push_back( new Figure( .3, _joints[1], _joints[2] ) );
	_figures.push_back( new Figure( .8, _joints[2], _joints[3] ) );
	_figures.push_back( new Figure( 1., _joints[3], _joints[4] ) );
	_figures.push_back( new Figure( .4, _joints[4], _joints[5] ) );
	_figures.push_back( new Figure( .2, _joints[5], _joints[6] ) );

	_arm = new Arm( _joints[0] );

	std::srand(std::time(nullptr)); // use current time as seed for random generator
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
    vec3 p1 = f->getStartJoint()->getPosition();
    vec3 p2 = f->getEndJoint()->getPosition();

    drawLine( p1, p2, color );
}
void drawIsolatedJoint( Joint * j, vec3 color ){
    vec3 p1 = j->getPosition();

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

void directCinematic( Joint * j, vec3 startPosition, vec3 startRotation, bool updateRotation ){
    j->setPosition( startPosition );
    if( updateRotation ){
        j->setRotation( startRotation );
    }

    if( j->getOutFigure() != NULL && j->getOutFigure()->getEndJoint() != NULL ){
        vec3 endPosition = getEndPoint( j->getOutFigure(), startPosition, startRotation );
        vec3 endRotation = startRotation.addition( j->getOutFigure()->getEndJoint()->getRotation() );
        directCinematic( j->getOutFigure()->getEndJoint(), endPosition, endRotation, updateRotation );
    }
}

bool isTargetReached( Joint * j, vec3 target ){
    return j->getPosition().soustraction( target ).norme() < 0.05;
}

void InverseCinematic( Joint * joint, vec3 targetPosition, bool fromBegin, vec3 accRotation ){
    joint->setPosition( targetPosition );
    Figure * figure = (fromBegin?joint->getOutFigure():joint->getInFigure());

    if( figure != NULL ){
        Joint * otherJoint = (fromBegin?figure->getEndJoint():figure->getStartJoint());
        if( otherJoint != NULL ){
            vec3 vecNewTarget = otherJoint->getPosition().soustraction( targetPosition ).normalized().multiplication( figure->getLength() );
            vec3 newTarget = targetPosition.addition( vecNewTarget );
            vec3 baseTarget = getEndPoint( figure, targetPosition, accRotation );

            float prod = baseTarget.soustraction( targetPosition ).normalized().produitScalaire( newTarget.soustraction( targetPosition ).normalized() );
            vec3 prodVet = baseTarget.soustraction( targetPosition ).normalized().produitVectoriel( newTarget.soustraction( targetPosition ).normalized() );
            float angle = (prodVet.getZ()<0?-1:1)*acos( prod )*180/PI;

            if( angle < -joint->getMaxDegrees() || angle > joint->getMaxDegrees() ){
                // Calculate new position
                newTarget = getEndPoint( figure, targetPosition, accRotation.addition( vec3((angle>0?1:-1)*joint->getMaxDegrees(),joint->getRotation().getY(),joint->getRotation().getZ()) ));

                prod = baseTarget.soustraction( targetPosition ).normalized().produitScalaire( newTarget.soustraction( targetPosition ).normalized() );
                prodVet = baseTarget.soustraction( targetPosition ).normalized().produitVectoriel( newTarget.soustraction( targetPosition ).normalized() );
                angle = (prodVet.getZ()<0?-1:1)*acos( prod )*180/PI;
            }
            joint->setRotation( vec3( angle, joint->getRotation().getY(), joint->getRotation().getZ() ) );
            accRotation.setX( accRotation.getX()+angle );
            // End angle

            return InverseCinematic( otherJoint, newTarget, fromBegin, accRotation );
        }
    }
}

void InverseCinematic( Arm * currentArm, vec3 target ){
    int cinemIT = 0;
    while( cinemIT < 5 && !( isTargetReached( currentArm->getEnd(), target ) ) ){
        InverseCinematic( currentArm->getEnd(),targetP,false,vec3(0,0,0) );
        InverseCinematic( currentArm->getBegin(),startP,true,vec3(0,0,0) );
        cinemIT++;
    }
}

bool isBetterSolution( Arm * currentArm, Arm * newArm, vec3 target ){
    Joint * endCurrent = currentArm->getEnd();
    float distCurrent = endCurrent->getPosition().soustraction( target ).norme();

    Joint * endNew = newArm->getEnd();
    float distNew = endNew->getPosition().soustraction( target ).norme();

    return( distCurrent > distNew );
}

float ObtainRandomAngle( Joint * j ){
    int randomAngle = std::rand()%360 - 180;
    if( randomAngle > 0 && randomAngle > j->getMaxDegrees() ){
        randomAngle = j->getMaxDegrees();
    }
    else if( randomAngle < 0 && randomAngle < -j->getMaxDegrees() ){
        randomAngle = -j->getMaxDegrees();
    }
    return randomAngle;
}

Arm * bestSolution( Arm * currentArm, vec3 target ){
    std::deque<Arm*> solutions;
    Arm * result = currentArm;

    // Creating random arms
    for( int i=0; i<nSolutions; i++ ){
        int randomAngle = ObtainRandomAngle( currentArm->getBegin() );
        solutions.push_back( new Arm( new Joint( vec3(0,0,0), vec3(randomAngle,0,0), currentArm->getBegin()->getMaxDegrees() ) ) );
    }
    for( int i=0; i<nSolutions; i++ ){
        Joint * joint = solutions[i]->getBegin();
        Joint * armJoint = currentArm->getBegin();
        while( armJoint != NULL ){
            if( armJoint->getOutFigure() != NULL && armJoint->getOutFigure()->getEndJoint() != NULL ){
                int randomAngle = ObtainRandomAngle( armJoint );
                Joint * newJoint = new Joint( vec3(0,0,0), vec3(randomAngle,0,0), armJoint->getOutFigure()->getEndJoint()->getMaxDegrees() );
                Figure * newFigure = new Figure( armJoint->getOutFigure()->getLength(), joint, newJoint );
                joint = newJoint;

                armJoint = armJoint->getOutFigure()->getEndJoint();
            }
            else{
                armJoint = NULL;
            }
        }
    }

    // Cinematic
    for( int i=0; i<nSolutions; i++ ){
        InverseCinematic( solutions[i], target );

        if( isBetterSolution( result, solutions[i], target ) ){
            delete( result );
            result = solutions[i];
        }
        else{
            delete( solutions[i] );
        }
    }

    return result;
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();

	drawPoint( targetP, green );
	drawPoint( startP, green );

    //directCinematic( joints[0], joints[0]->getPosition(), joints[0]->getRotation(), true );
    //draw( joints[0], white );

    InverseCinematic( _arm, targetP );
    //draw( _arm->getBegin(), red );

    _arm = bestSolution( _arm, targetP );
    draw( _arm->getBegin(), red );

    // The white lines represent the one built based on the angles [To check if the angles are correct : the white lines will follow the red ones]
    /*directCinematic( arm->getBegin(), arm->getBegin()->getPosition(), arm->getBegin()->getRotation(), false );
    draw( arm->getBegin(), white );*/

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
        _joints[ selectedJoint ]->rotate(Joint::Axe::X, -10);
        break;
    case '+':
        _joints[ selectedJoint ]->rotate(Joint::Axe::X, 10);
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
