#include "Figure.h"
#include <iostream>

Figure::Figure( float length, Joint * startJoint, Joint * endJoint ){
    this->length = length;
    this->startJoint = startJoint;
    if( startJoint != NULL ){ startJoint->setOutFigure( this ); }
    this->endJoint = endJoint;
}

Figure::~Figure()
{
    //dtor
}
float Figure::getLength()
{
    return this->length;
}

Joint * Figure::getStartJoint()
{
    return this->startJoint;
}

Joint * Figure::getEndJoint()
{
    return this->endJoint;
}
