#include <string>
#include <sstream>
#include <iostream>
using namespace std;
#include "vec3.h"

#define FOR(i,a) for(int i=0;i<a;i++)
#define PRINT(s) cout << s << endl;

#pragma once
bool equalDouble(double value1, double value2);

std::string doubleToStr( double value );

double clamp(double valeur, double min, double max);

//vec3 clamp(vec3 valeur, double min, double max);
vec3 getNormal( vec3 p1, vec3 p2, vec3 p3 );
