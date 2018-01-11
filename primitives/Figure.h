#ifndef FIGURE_H
#define FIGURE_H

#include <string>
#include "Joint.h"
class Joint;

class Figure
{
    Joint * startJoint;
    Joint * endJoint;

    float length;

    public:
        /** Default constructor */
        Figure( float length, Joint * startJoint, Joint * endJoint );
        /** Default destructor */
        virtual ~Figure();

        float getLength();
        Joint * getStartJoint();
        Joint * getEndJoint();
};

#endif // FIGURE_H
