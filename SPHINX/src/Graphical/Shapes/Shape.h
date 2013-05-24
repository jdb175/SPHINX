#ifndef SHAPE_H
#define SHAPE_H


#include <QPainter>


class Shape
{

public:
    virtual void Draw (double x, double y, float radius, QPainter *p) = 0;
    virtual bool IncludesPoint (double x, double y) = 0;
};

#endif // SHAPE_H
