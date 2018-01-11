#ifndef JOINT_H
#define JOINT_H

#include "../utils/vec3.h"
#include "Figure.h"
class Figure;

class Joint
{
    public:
        enum Axe{ X, Y, Z };

    private:
        vec3 position;
        vec3 rotation;
        Figure * inFigure;
        Figure * outFigure;

    public:
        /** Default constructor */
        Joint( vec3 startPosition );
        Joint( vec3 startPosition, vec3 startRotation );
        /** Default destructor */
        virtual ~Joint();

        void rotate( Axe axe, float degrees );

        vec3 getPosition();
        vec3 getRotation();
        Figure * getInFigure();
        Figure * getOutFigure();
        void setInFigure( Figure * f );
        void setOutFigure( Figure * f );

    private:
};

#endif // JOINT_H
