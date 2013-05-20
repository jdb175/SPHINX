#include <algorithm>

#include "paras/Rule.hpp"
#include "graphical/StableRegion.hpp"

using namespace PARASProgram;
using namespace PARASProgram::Graphical;

StableRegion::StableRegion (ColorMap *cMap, double sup, double conf, set<Rule*> *allRules, set<Rule*> *uniqueRules, set<Rule*> *allRules_nr, set<Rule*> *uniqueRules_nr)
{
    this->colorMap = cMap;
    support = sup;
    confidence = conf;
    this->allRules = allRules;
    this->uniqueRules = uniqueRules;
    this->allRules_nr = allRules_nr;
    this->uniqueRules_nr = uniqueRules_nr;
    selected = false;
}

StableRegion::~StableRegion(){}

void StableRegion::fullDelete()
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


void StableRegion::setRect(QRect r){
    rect = r;
}

void StableRegion::draw(QPainter *p)
{
    //Draws the stable region rectangle
    //updateRect();
    if(!rect.isEmpty()){
        p->save();
        p->setBrush(curColor);
        p->drawRect(rect);
        p->restore();
    }
}

void StableRegion::deselect()
{
    //Reverts the color to our base
    selected = false;
}

void StableRegion::highlight()
{
    //Reverts the color to our base
    if(selected)
        return;

    curColor = QColor(200,200,200,255);
}

void StableRegion::select(bool secondary)
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

set<Rule*> *StableRegion::getRules(RuleMode mode,  bool includeRedundancies)
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

bool StableRegion::operator < (StableRegion s){
    return (support < s.support || (confidence < s.confidence && support == s.support));
}

bool StableRegion::operator > (StableRegion s){
    return (support > s.support || (confidence > s.confidence && support == s.support));
}
