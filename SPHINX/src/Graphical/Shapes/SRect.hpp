#ifndef SRECT_HPP
#define SRECT_HPP

#include <QPainter>
#include <QRect>
#include "Graphical/Shapes/Shape.h"

class SRect : public Shape
{
public:
    SRect();
    void Draw (double x, double y, float radius, QPainter *p);
    bool IncludesPoint (double x, double y);
private:
    QRect r;
};

#endif // SRECT_HPP
