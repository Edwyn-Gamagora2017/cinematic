#include "Arm.h"
Arm::Arm()
{
    this->startJoint = NULL;
}

Arm::Arm( Joint * start )
{
    this->startJoint = start;
}

Arm::~Arm()
{
    Joint * joint = this->startJoint;
    Figure * figure = NULL;
    if( joint != NULL ){
        figure = joint->getOutFigure();
        delete( joint );
    }
    joint = NULL;

    while( figure != NULL ){
        joint = figure->getEndJoint();
        delete( figure );
        figure = NULL;
        if( joint != NULL ){
            figure = joint->getOutFigure();
            delete( joint );
            joint = NULL;
        }
    }
}

void Arm::setBegin( Joint * start ){
    this->startJoint = start;
}

Joint* Arm::getBegin()
{
    return this->startJoint;
}

Joint* Arm::getEnd()
{
    Joint * joint = this->startJoint;
    Figure * figure = NULL;
    if( joint != NULL ){
        figure = joint->getOutFigure();
    }

    while( figure != NULL ){
        if( figure->getEndJoint() != NULL ){
            joint = figure->getEndJoint();
            figure = joint->getOutFigure();
        }
    }
    return joint;
}

int Arm::getJointCount(){
    int result = 0;

    Joint * joint = this->startJoint;
    Figure * figure = NULL;

    if( joint != NULL ){
        figure = joint->getOutFigure();
        result++;
    }

    while( figure != NULL ){
        joint = figure->getEndJoint();
        if( joint != NULL ){
            figure = joint->getOutFigure();
            result++;
        }
    }
    return result;
}
