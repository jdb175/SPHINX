#ifndef STABLEREGION_H
#define STABLEREGION_H

#include <set>

#include "graphical/ruleview/singlerule/glyph/Glyph.hpp"
#include "paras/Nugget.hpp"
#include "graphical/color/ColorMap.h"
#include "graphical/Shapes/Shape.h"


using namespace std;

namespace SPHINXProgram
{
    namespace Graphical
    {
        enum RuleMode
        {
            UNIQUE = 0,
            ALL = 1
        };

        class NPoint
        {
            public:
                NPoint (ColorMap *cMap, double sup, double conf, set<Nugget*> *allRules, set<Nugget*> *uniqueRules, set<Nugget*> *allRules_nr, set<Nugget*> *uniqueRules_nr);
                ~NPoint();
                void fullDelete();
                void draw(QPainter *p);
                void setXY(int xPos, int yPos);
                bool isClicked(double sup, double conf);
                double support;
                double confidence;
                int radius;
                int xPos;
                int yPos;
                bool selected;
                void deselect();
                void select(bool secondary);
                void highlight();
                QColor baseColor;
                QColor curColor;
                set<Nugget*> *uniqueRules;
                set<Nugget*> *uniqueRules_nr;
                set<Nugget*> *allRules;
                set<Nugget*> *allRules_nr;
                set<Nugget*> *getRules(RuleMode mode, bool includeRedundancies);

                bool operator < (NPoint);
                bool operator > (NPoint);

            protected:
                Shape *shape;
                ColorMap *colorMap;

        };
    }
}


#endif // STABLEREGION_H
