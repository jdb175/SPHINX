#ifndef RULEPANEL_HPP
#define RULEPANEL_HPP

#include <QWidget>
#include <QFrame>
#include <QRadioButton>
#include <QScrollArea>
#include "graphical/EventController.hpp"
#include "graphical/ruleview/singlerule/SingleView.hpp"
#include "graphical/ruleview/doublerule/DoubleRuleTable.hpp"
#include "graphical/color/ColorMap.h"

namespace PARASProgram
{
    namespace Graphical
    {
        class RuleView : public QFrame, public EventListener
        {
                Q_OBJECT
            public:
                explicit RuleView(QWidget *parent, EventController *evCont, ColorMap *coloring);

                //event listener
                void selectStableRegions(StableRegion *primarySR, StableRegion *secondarySR);
                void applicationSizeChanged(int width, int height);
                void setRuleMode(RuleMode ruleMode);
                void updateIndexInfo(IndexUpdateEvent e);
                void updateRedundancy(bool include);
                void closeIndex();
                void paintEvent(QPaintEvent *e);
            signals:

            public slots:

            private:
                SingleView *singRT;
                DoubleRuleTable *doubRT;

                ColorMap *colorMapping;
        };
    }
}


#endif // RULEPANEL_HPP
