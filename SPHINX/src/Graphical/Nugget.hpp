#ifndef STABLEREGION_H
#define STABLEREGION_H

#include <set>

#include "graphical/ruleview/singlerule/glyph/Glyph.hpp"
#include "paras/Rule.hpp"
#include "graphical/color/ColorMap.h"


using namespace std;

namespace PARASProgram
{
    namespace Graphical
    {
        enum RuleMode
        {
            UNIQUE = 0,
            ALL = 1
        };

        class Nugget
        {
            public:
                Nugget (ColorMap *cMap, double sup, double conf, set<Rule*> *allRules, set<Rule*> *uniqueRules, set<Rule*> *allRules_nr, set<Rule*> *uniqueRules_nr);
                ~Nugget();
                void fullDelete();
                void draw(QPainter *p);
                void setRect(QRect r);
                bool isClicked(double sup, double conf);
                double support;
                double confidence;
                bool selected;
                void deselect();
                void select(bool secondary);
                void highlight();
                QColor baseColor;
                QColor curColor;
                set<Rule*> *uniqueRules;
                set<Rule*> *uniqueRules_nr;
                set<Rule*> *allRules;
                set<Rule*> *allRules_nr;
                set<Rule*> *getRules(RuleMode mode, bool includeRedundancies);

                bool operator < (Nugget);
                bool operator > (Nugget);

            protected:
                QRect rect;
                ColorMap *colorMap;

        };
    }
}


#endif // STABLEREGION_H
