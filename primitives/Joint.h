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
        vec3 initialRotation;
        Figure * inFigure;
        Figure * outFigure;
        float maxDegrees;

    public:
        /** Default constructor */
        Joint( vec3 startPosition );
        Joint( vec3 startPosition, vec3 startRotation );
        Joint( vec3 startPosition, vec3 startRotation, float maxDegrees );
        /** Default destructor */
        virtual ~Joint();

        void rotate( Axe axe, float degrees );

        vec3 getPosition();
        vec3 getRotation();
        vec3 getInitialRotation();
        Figure * getInFigure();
        Figure * getOutFigure();
        float getMaxDegrees();
        void setPosition( vec3 pos );
        void setRotation( vec3 rot );
        void setInFigure( Figure * f );
        void setOutFigure( Figure * f );

    private:
};

#endif // JOINT_H
