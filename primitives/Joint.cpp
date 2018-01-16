#include "Joint.h"

#include<iostream>

Joint::Joint( vec3 startPosition, vec3 startRotation, float maxDegrees ){
    this->position = startPosition;
    this->rotation = startRotation;
    this->initialRotation = startRotation;
    this->maxDegrees = maxDegrees;
    this->inFigure = NULL;
    this->outFigure = NULL;
}

Joint::Joint(vec3 startPosition, vec3 startRotation )
: Joint::Joint( startPosition, startRotation, 360 ){}

Joint::Joint(vec3 startPosition)
: Joint::Joint( startPosition, vec3(0,0,0) ){}

void Joint::rotate(Axe axe, float degrees)
{
    vec3 newRotation( (axe==X)?degrees:0, (axe==Y)?degrees:0, (axe==Z)?degrees:0 );
    this->rotation = this->rotation.addition( newRotation );
}

vec3 Joint::getPosition(){
    return this->position;
}
vec3 Joint::getRotation(){
    return this->rotation;
}
vec3 Joint::getInitialRotation(){
    return this->initialRotation;
}
float Joint::getMaxDegrees(){
    return this->maxDegrees;
}
void Joint::setPosition( vec3 pos ){
    this->position = pos;
}
void Joint::setRotation( vec3 rot ){
    this->rotation = rot;
}
void Joint::setInFigure( Figure * f ){
    this->inFigure = f;
}
void Joint::setOutFigure( Figure * f ){
    this->outFigure = f;
}
Figure * Joint::getInFigure(){
    return this->inFigure;
}
Figure * Joint::getOutFigure(){
    return this->outFigure;
}

Joint::~Joint()
{
    //dtor
}
