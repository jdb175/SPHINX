#include <math.h>
#include <algorithm>

#include "graphical/ruleview/singlerule/glyph/Glyph.hpp"
#include "paras/Attribute.hpp"


#define PI 3.14159265


using namespace PARASProgram;
using namespace PARASProgram::Graphical;

Glyph::Glyph(ColorMap *coloring)
{
    this->x = 0;
    this->gx = 0;
    this->y = 0;
    this->gy = 0;
    this->radius = 0;
    this->gRadius = 0;
    this->simval = 0;
    this->lengthArray = new vector<double>();
    this->antecedentIndices = new vector<int>();
    this->consequentIndices = new vector<int>();
    this->missingIndices = new vector<int>();
    this->arcPoints = new vector<QPoint>();
    this->drawnRays = new vector<QLineF>();
    this->originPoint = QPoint();
    this->cMap = coloring;
    this->legend = false;
    this->border = false;
}

Glyph::Glyph(Rule *rule, vector<Attribute*> *attributes, ColorMap *coloring)
{
    this->x = 0;
    this->gx = 0;
    this->y = 0;
    this->gy = 0;
    this->radius = 0;
    this->gRadius = 0;
    this->rule = rule;
    this->simval = 0;
    this->attributesList = attributes;
    this->lengthArray = new vector<double>();
    this->antecedentIndices = new vector<int>();
    this->consequentIndices = new vector<int>();
    this->missingIndices = new vector<int>();
    this->arcPoints = new vector<QPoint>();
    this->drawnRays = new vector<QLineF>();
    this->originPoint = QPoint();
    this->cMap = coloring;
    this->border = false;
    this->legend = false;
    this->buildGlyph();
}

Glyph::~Glyph()
{
    lengthArray->clear();
    delete lengthArray;
    antecedentIndices->clear();
    delete antecedentIndices;
    consequentIndices->clear();
    delete consequentIndices;
    missingIndices->clear();
    delete missingIndices;
    arcPoints->clear();
    delete arcPoints;
    this->drawnRays->clear();
    delete drawnRays;
}


void Glyph::setLegend(bool leg)
{
    this->legend = true;
}

void Glyph::setNaivePosition(double x, double y)
{
    this->x = x;
    this->y = y;
}


void Glyph::setGroupedPosition(double x, double y)
{
    this->gx = x;
    this->gy = y;
}

void Glyph::setNaiveRadius(double radius)
{
    this->radius = radius;
}

void Glyph::setGroupedRadius(double radius)
{
    this->gRadius = radius;
}

void Glyph::setAttributes(vector<Attribute*> *attr)
{
    this->attributesList = attr;
    buildGlyph();
}


void Glyph::buildLegend()
{
    int count = 0;
    for(vector<Attribute*>::iterator a = attributesList->begin(); a != attributesList->end(); ++a)
    {

        lengthArray->push_back(.25);
        consequentIndices->push_back(count);
        arcPoints->push_back(QPoint(-1,-1));
        count++;
    }
}

void Glyph::buildGlyph()
{
    lengthArray->clear();
    antecedentIndices->clear();
    consequentIndices->clear();
    missingIndices->clear();
    arcPoints->clear();
    this->simval = 0;

    if(legend)
    {
        buildLegend();
        return;
    }

    if(rule == NULL || this->attributesList == NULL) return;

    vector<string*> *xStr = rule->getX();
    vector<string*> *yStr = rule->getY();
    int currIndex = 0;

    //draw a line for each attribute.
    for(vector<Attribute*>::iterator a = attributesList->begin(); a != attributesList->end(); ++a)
    {
        Attribute *curAttr = *a;
        bool found = false;
        bool antecedent = true;
        string foundMember;

        //check x string to find the member of the attribute.
        for(vector<string*>::iterator k = xStr->begin(); k != xStr->end() && !found; ++k)
        {
            if(curAttr->isMember(*(*k)))
            {
                foundMember = *(*k);
                found = true;
            }
        }

        //check y string to find member of the attribute.
        for(vector<string*>::iterator k = yStr->begin(); k != yStr->end() && !found; ++k)
        {
            if(curAttr->isMember(*(*k)))
            {
                foundMember = *(*k);
                found = true;
                antecedent = false;
            }
        }

        //length of the ray is equivalent to the radius * the relative position
        double length = curAttr->getRelativePosition(foundMember);
        if(found)
        {
            lengthArray->push_back(length);
            simval += length;
            if(antecedent)
            {
                antecedentIndices->push_back(currIndex);
            }
            else
            {
                consequentIndices->push_back(currIndex);
            }
        }
        else
        {
            lengthArray->push_back(-1);
            simval += -1;
            missingIndices->push_back(currIndex);
        }
        arcPoints->push_back(QPoint(-1, -1));
        currIndex++;
    }

}

void Glyph::draw(QPainter *painter, bool showConnections, bool fillingShape, bool drawingMissing, bool grouped, bool showLabels)
{
    if(lengthArray->size() == 0) return;
    drawnRays->clear();

    this->drawnX = x;
    this->drawnY = y;
    this->drawnRadius = radius;

    if(grouped)
    {
        this->drawnX = gx;
        this->drawnY = gy;
        this->drawnRadius = gRadius;
    }

    this->originPoint = QPoint(drawnX + (drawnRadius), drawnY + drawnRadius);

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    QFont newFont;
    int fontSize = drawnRadius / 5;
    if(fontSize < 5) showLabels = false;
    newFont.setPointSize(fontSize);
    painter->setFont(newFont);

    QPen glyphPen(QBrush(Qt::SolidPattern), 1);

    if(border) painter->drawRect(QRect(drawnX, drawnY, drawnRadius*2, drawnRadius*2));

    double deg_inc = 360 / (double)attributesList->size();

    glyphPen.setColor(cMap->glyphAntecedentColor);
    painter->setPen(glyphPen);
    drawRays(painter, antecedentIndices, deg_inc, showLabels);

    glyphPen.setColor(cMap->glyphConsequentColor);
    painter->setPen(glyphPen);
    drawRays(painter, consequentIndices, deg_inc, showLabels);


    if(showConnections)
    {
        glyphPen.setColor(cMap->glyphConnectColor);
        painter->setPen(glyphPen);

        QPoint lastPoint;
        QPoint firstPoint;
        bool firstFound = false;

        for(vector<QPoint>::iterator p = arcPoints->begin(); p != arcPoints->end(); ++p)
        {
            QPoint arcPoint = *p;
            if(arcPoint.x() == -1) continue;

            if(firstFound)
            {
                QLineF toDraw(lastPoint, arcPoint);
                if(!intersectsWithPreviouslyDrawn(toDraw)) painter->drawLine(lastPoint, arcPoint);
            }
            else
            {
                firstPoint = arcPoint;
                firstFound = true;
            }

            lastPoint = arcPoint;
        }

        QLineF toDraw(lastPoint, firstPoint);
        if(!intersectsWithPreviouslyDrawn(toDraw)) painter->drawLine(lastPoint, firstPoint);
    }

    if(drawingMissing)
    {
        QColor missing = cMap->glyphMissingColor;
        missing.setAlpha(75);
        glyphPen.setColor(missing);
        glyphPen.setWidth(0);
        painter->setPen(glyphPen);

        drawRays(painter, missingIndices, deg_inc, showLabels);
    }

    if(fillingShape)
    {

        QPoint previousPoint;
        QPoint firstPoint;
        bool firstFound = false;

        for(int i = 0; i < arcPoints->size(); ++i)
        {
            QPoint arcPoint = arcPoints->at(i);

            if(arcPoint.x() == -1) continue;

            if(firstFound)
            {
                //figure out the color of the filled shape.
                //Should be the color of the ray to the right.
                QColor coloring = cMap->glyphConsequentColor;

                if(std::find(antecedentIndices->begin(), antecedentIndices->end(), i-1) != antecedentIndices->end())
                {
                    coloring = cMap->glyphAntecedentColor;
                }

                //build the polygon to fill the shape
                //build the polygon.
                QVector<QPoint> points;
                points.push_back(originPoint);
                points.push_back(previousPoint);
                points.push_back(arcPoint);
                QPainterPath tmp;
                tmp.addPolygon(QPolygon(points));
                painter->fillPath(tmp, QBrush(coloring));
            }
            else
            {
                firstFound = true;
                firstPoint = arcPoint;
            }

            previousPoint = arcPoint;
        }

        if(firstFound)
        {
            QColor coloring = cMap->glyphConsequentColor;
            if(std::find(antecedentIndices->begin(), antecedentIndices->end(), arcPoints->size() - 1) != antecedentIndices->end())
                coloring = cMap->glyphAntecedentColor;

            //build the polygon.
            QVector<QPoint> points;
            points.push_back(originPoint);
            points.push_back(previousPoint);
            points.push_back(firstPoint);
            QPainterPath tmp;
            tmp.addPolygon(QPolygon(points));
            painter->fillPath(tmp, QBrush(coloring));
        }
    }

    painter->restore();
}

bool Glyph::contains(double x, double y)
{
    bool containsX = x >= this->drawnX && x <= (this->drawnX + (2 * drawnRadius));
    bool containsY = y >= this->drawnY && y <= (this->drawnY + (2 * drawnRadius));

    return containsX && containsY;
}

void Glyph::drawRays(QPainter *painter, vector<int> *indices, double deg_increment, bool showLabels)
{
    for(vector<int>::iterator indicesIter = indices->begin(); indicesIter != indices->end(); ++indicesIter)
    {
        int index = *indicesIter;
        double arrayLength = lengthArray->at(index);
        double length = drawnRadius * arrayLength + drawnRadius;
        if(arrayLength == -1) length = drawnRadius * 2;

        double degPosition = deg_increment * index;
        double arcX = (drawnX + drawnRadius) + ((length / 2) * (cos(PI * degPosition / 180)));
        double arcY = (drawnY + drawnRadius) + ((length / 2) * (sin(PI * degPosition / 180)));

        QLineF ray(originPoint, QPoint(arcX, arcY));
        painter->drawLine(ray);

        if(arrayLength != -1) //if ray isnt missing, add it to the arcpoints and draw the name.
        {
            if(showLabels)
            {
                int numAttributes = antecedentIndices->size() + consequentIndices->size();
                int showBase = ceil((double)numAttributes / 10);

                double labelX = arcX;
                double labelY = arcY;
                bool leftOfOrigin = arcX <= originPoint.x();
                bool belowOrigin = arcY >= originPoint.y();

                if(belowOrigin && leftOfOrigin)
                {
                    labelY += drawnRadius / 7;
                    labelX -= drawnRadius / 6;
                }
                else
                {
                    if(leftOfOrigin) labelX -= drawnRadius / 4;
                    if(belowOrigin) labelY += drawnRadius / 6;
                }
                if(index % showBase == 0)
                    painter->drawText(QPoint(labelX, labelY), QString::number(index + 1));
            }
            drawnRays->push_back(ray);
            QPoint *point = &arcPoints->at(index);
            point->setX(arcX);
            point->setY(arcY);
        }
    }
}

Glyph& Glyph::operator=(const Glyph &source)
{
    this->rule = source.rule;
    this->attributesList = source.attributesList;
    this->buildGlyph();
}

bool Glyph::intersectsWithPreviouslyDrawn(QLineF toCheck)
{
    for(vector<QLineF>::iterator prev = drawnRays->begin(); prev != drawnRays->end(); ++prev)
    {
        QLineF drawn = *prev;
        QPointF intersectionPoint;
        QLineF::IntersectType in = drawn.intersect(toCheck, &intersectionPoint);

        if(in == QLineF::BoundedIntersection)
        {
            if(!(intersectionPoint == drawn.p1() || intersectionPoint == drawn.p2())) return true;
        }
    }

    return false;
}

vector<double> *Glyph::getData()
{
    return this->lengthArray;
}
