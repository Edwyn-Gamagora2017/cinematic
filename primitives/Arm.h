#ifndef ARM_H
#define ARM_H

#include <deque>
#include "Joint.h"
#include "Figure.h"

class Arm
{
    protected:
        Joint * startJoint;

    public:
        /** Default constructor */
        Arm();
        Arm( Joint * start );
        /** Default destructor */
        virtual ~Arm();

        void setBegin( Joint * start );
        Joint * getBegin();
        Joint * getEnd();

        int getJointCount();
};

#endif // ARM_H
