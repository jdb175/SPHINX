#ifndef STRIANGLE_H
#define STRIANGLE_H

#include "src/Graphical/Shapes/Shape.h"
#include <QPainter>
#include <QPainterPath>

class STriangle : public Shape
{
public:
    STriangle();
    void Draw (double x, double y, float radius, QPainter *p);
    bool IncludesPoint (double x, double y);
    ~STriangle();

private:
    QPainterPath thePath;
};

#endif // STRIANGLE_H
