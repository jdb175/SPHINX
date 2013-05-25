#include <algorithm>

#include "paras/Rule.hpp"
#include "graphical/Nugget.hpp"
#include "graphical/Shapes/SCircle.h"
#include "graphical/Shapes/SRect.hpp"

#include <QDebug>

using namespace SPHINXProgram;
using namespace SPHINXProgram::Graphical;

Nugget::Nugget (ColorMap *cMap, double sup, double conf, set<Rule*> *allRules, set<Rule*> *uniqueRules, set<Rule*> *allRules_nr, set<Rule*> *uniqueRules_nr)
{
    this->colorMap = cMap;
    support = sup;
    confidence = conf;
    this->allRules = allRules;
    this->uniqueRules = uniqueRules;
    this->allRules_nr = allRules_nr;
    this->uniqueRules_nr = uniqueRules_nr;
    if((float)rand()/(float)RAND_MAX > 0.5) {
        shape = (Shape*) new SCircle();
    } else {
        shape = (Shape*) new SRect();
    }
    selected = false;
    radius = 10;
}

Nugget::~Nugget(){}

void Nugget::fullDelete()
{
    allRules->clear();
    uniqueRules->clear();
    allRules_nr->clear();
    uniqueRules_nr->clear();

    set<Rule*>().swap(*allRules_nr);
    set<Rule*>().swap(*uniqueRules_nr);
    set<Rule*>().swap(*allRules);
    set<Rule*>().swap(*uniqueRules);
    delete this;
}


void Nugget::setXY(int xPos, int yPos){
    this->xPos = xPos;
    this->yPos = yPos;
}

void Nugget::draw(QPainter *p)
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

void Nugget::deselect()
{
    //Reverts the color to our base
    selected = false;
}

void Nugget::highlight()
{
    //Reverts the color to our base
    if(selected)
        return;

    curColor = QColor(200,200,200,255);
}

void Nugget::select(bool secondary)
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

set<Rule*> *Nugget::getRules(RuleMode mode,  bool includeRedundancies)
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

bool Nugget::isClicked(double sup, double conf) {
    return shape->IncludesPoint(sup, conf);
}

bool Nugget::operator < (Nugget s){
    return (support < s.support || (confidence < s.confidence && support == s.support));
}

bool Nugget::operator > (Nugget s){
    return (support > s.support || (confidence > s.confidence && support == s.support));
}
