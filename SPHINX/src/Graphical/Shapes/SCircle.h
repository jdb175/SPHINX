#ifndef SCIRCLE_H
#define SCIRCLE_H

#include <QPainter>
#include "Graphical/Shapes/Shape.h"

class SCircle : public Shape
{
public:
    SCircle();
    void Draw (double x, double y, float radius, QPainter *p);
    bool IncludesPoint (double x, double y);
    ~SCircle();

private:
    float radius;
    double xPos;
    double yPos;
};

#endif // SCIRCLE_H
