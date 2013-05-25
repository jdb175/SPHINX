#include <QVector>
#include <QPolygonF>
#include <QPointF>
#include <QPainterPathStroker>
#include "math.h"
#include "STriangle.h"

STriangle::STriangle(){
}

void STriangle::Draw (double x, double y, float radius, QPainter *p){
    thePath = QPainterPath();
    //Create an equlateral triangle
    float halfAlt = radius*sqrt(3)/2;
    QVector<QPointF> points;
    points.push_back(QPointF(x-radius, y+halfAlt));
    points.push_back(QPointF(x+radius, y+halfAlt));
    points.push_back(QPointF(x, y-halfAlt));
    points.push_back(QPointF(x-radius, y+halfAlt));
    QPolygonF trianglePoly(points);

    thePath.addPolygon(trianglePoly);
    p->drawPath(thePath);
}

bool STriangle::IncludesPoint (double x, double y){
    return thePath.contains(QPointF(x,y));
}
