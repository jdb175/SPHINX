#include "SRect.hpp"

#include <QRect>

SRect::SRect()
{
}

void SRect::Draw (double x, double y, float radius,  QPainter *p){
    r = QRect(QPoint(x-radius,y-radius), QPoint(x+radius,y+radius));
    p->drawRect(r);
}

bool SRect::IncludesPoint (double x, double y){
    return x >= r.bottomLeft().x() && x <= r.bottomRight().x() && y >= r.topLeft().y() && y <= r.bottomLeft().y();
}

SRect::~SRect() {}
