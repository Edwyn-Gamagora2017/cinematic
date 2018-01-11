#include "Joint.h"

#include<iostream>

Joint::Joint(vec3 startPosition, vec3 startRotation )
{
    this->position = startPosition;
    this->rotation = startRotation;
    this->relPosition = startPosition;
    this->relRotation = startRotation;
}

Joint::Joint(vec3 startPosition)
{
    this->position = startPosition;
    this->rotation = vec3(0,0,0);
}

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
vec3 Joint::getRelPosition(){
    return this->relPosition;
}
vec3 Joint::getRelRotation(){
    return this->relRotation;
}
void Joint::setPosition( vec3 pos ){
    this->position = pos;
}
void Joint::setRotation( vec3 rot ){
    this->rotation = rot;
}
void Joint::setRelPosition( vec3 pos ){
    this->relPosition = pos;
}
void Joint::setRelRotation( vec3 rot ){
    this->relRotation = rot;
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
