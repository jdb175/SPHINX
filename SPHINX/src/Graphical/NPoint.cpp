#include <algorithm>

#include "paras/Nugget.hpp"
#include "graphical/NPoint"
#include "graphical/Shapes/SCircle.h"
#include "graphical/Shapes/STriangle.h"
#include "graphical/Shapes/SRect.hpp"

#include <QDebug>

using namespace SPHINXProgram;
using namespace SPHINXProgram::Graphical;

NPoint::NPoint (ColorMap *cMap, double sup, double conf, set<Nugget*> *allRules, set<Nugget*> *uniqueRules, set<Nugget*> *allRules_nr, set<Nugget*> *uniqueRules_nr)
{
    this->colorMap = cMap;
    support = sup;
    confidence = conf;
    this->allRules = allRules;
    this->uniqueRules = uniqueRules;
    this->allRules_nr = allRules_nr;
    this->uniqueRules_nr = uniqueRules_nr;

    //temporary random shapes
    float rVal = (float)rand()/(float)RAND_MAX;
    if(rVal > 0.66) {
        shape = (Shape*) new SCircle();
    } else if (rVal > 0.33) {
        shape = (Shape*) new SRect();
    } else {
        shape = (Shape*) new STriangle();
    }
    selected = false;
    radius = 10;
}

NPoint::~NPoint(){}

void NPoint::fullDelete()
{
    allRules->clear();
    uniqueRules->clear();
    allRules_nr->clear();
    uniqueRules_nr->clear();

    set<Nugget*>().swap(*allRules_nr);
    set<Nugget*>().swap(*uniqueRules_nr);
    set<Nugget*>().swap(*allRules);
    set<Nugget*>().swap(*uniqueRules);
    delete this;
}


void NPoint::setXY(int xPos, int yPos){
    this->xPos = xPos;
    this->yPos = yPos;
}

void NPoint::draw(QPainter *p)
{
    //Draws the stable region rectangle
    //updateRect();
    p->save();
    p->setBrush(curColor);
    shape->Draw(xPos, yPos, radius, p);
    p->restore();
    /*if(!
     *rect.isEmpty()){
        p->save();
        p->setBrush(curColor);
        p->drawRect(rect);
        p->restore();
    }*/
}

void NPoint::deselect()
{
    //Reverts the color to our base
    selected = false;
}

void NPoint::highlight()
{
    //Reverts the color to our base
    if(selected)
        return;

    curColor = QColor(200,200,200,255);
}

void NPoint::select(bool secondary)
{
    //Changes color to indicate selection
    selected = true;
    if(secondary)
    {
        curColor = QColor(155,155,155,255);
    }
    else
    {
        curColor = QColor(0,0,0,255);
    }
}

set<Nugget*> *NPoint::getRules(RuleMode mode,  bool includeRedundancies)
{
    switch(mode)
    {
        case UNIQUE:
            if(includeRedundancies)
                return uniqueRules;
            else
                return uniqueRules_nr;
        case ALL:
        if(includeRedundancies)
            return allRules;
        else
            return allRules_nr;
    }
}

bool NPoint::isClicked(double sup, double conf) {
    return shape->IncludesPoint(sup, conf);
}

bool NPoint::operator < (NPoint s){
    return (support < s.support || (confidence < s.confidence && support == s.support));
}

bool NPoint::operator > (NPoint s){
    return (support > s.support || (confidence > s.confidence && support == s.support));
}
