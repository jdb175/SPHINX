#include "SCircle.h"
#include <math.h>

SCircle::SCircle()
{
}

void SCircle::Draw (double x, double y, float radius, QPainter *p){
    //draw the circle
    p->drawEllipse(x-radius,y-radius,radius*2,radius*2);
    this->radius = radius;
    this->xPos = x;
    this->yPos = y;
}

bool SCircle::IncludesPoint (double x, double y) {
    return (pow(y-yPos,2) + pow(x-xPos, 2)) <= pow(radius, 2);
}

SCircle::~SCircle() {}
