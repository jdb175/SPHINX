#ifndef PSPACEGRAPH_H
#define PSPACEGRAPH_H

#include <QWidget>
#include <QtGui>
#include <QFrame>
#include <QWheelEvent>
#include <QButtonGroup>
#include <QRadioButton>
#include <QSlider>
#include <QScrollArea>
#include <QLineEdit>

#include "graphical/color/ColorMap.h"
#include "graphical/StableRegion.hpp"
#include "graphical/EventController.hpp"
#include "graphical/color/ColorManager.h"
#include "graphical/color/ColorSelector.h"

#define TRUNCATE_INTERVAL 10
#define MAX_TRUNCATE_VAL 1000
#define MIN_TRUNCATE_VAL 10


namespace PARASProgram
{
    namespace Graphical
    {
        class PSpaceGraph : public QWidget, public EventListener
        {
           Q_OBJECT
           public:
            PSpaceGraph(QWidget *parent, EventController *controller, ColorMap *cMap, ColorSelector *cSelect);
            ~PSpaceGraph();
            bool secondarySelect;

            void setConfig(bool bgLines, bool skyline, bool include, RuleMode ruleMode, int cardinality, int granularity);
            void selectStableRegions(StableRegion *primarySR, StableRegion *secondarySR);
            void applicationSizeChanged(int width, int height);
            void updateRedundancy(bool include);
            void setRuleMode(RuleMode ruleMode);
            void updateIndexInfo(IndexUpdateEvent e);
            void closeIndex();
            void setCursorHiding(bool b);

            RuleMode ruleMode;
            int skyLineCardinality;
            bool useCardinality;
            bool includeRedundancies;
            bool mouseDragged;
            bool bgLines;
            bool skyline;

           public slots:
            void slotRMode();
            void colorChooser();
            void updateGranularity();
            void updateGridLines();
            void updateSkyline();
            void setRedundancy();
            void SupConfInput();
            void updateShownGranularity(int g);
           private:
            void buildZoomFrame();
            void updateStableRect(StableRegion *s);
            void paintEvent(QPaintEvent *);
            void drawGraph (QPainter *painter);
            void drawStableRegions(QPainter *painter);
            void drawSkyline(QPainter *painter);
            void drawCardinalitySkyline(QPainter *painter);
            void drawCursorLines(QPainter *painter);
            void highlightDominatingRegions();
            StableRegion *getClickedRegion(QMouseEvent *event);
            StableRegion *getRegionFromPoint(double sup, double conf);
            void normalizeZoom();
            void panDragged(QMouseEvent *event, QPoint dragLast);
            void zoomScrolled(QWheelEvent* ev);
            void resetGraph();
            QPoint getTopRightFromSupConf(double support, double confidence);


            virtual void wheelEvent(QWheelEvent* ev);
            virtual void mousePressEvent(QMouseEvent *event);
            virtual void mouseMoveEvent(QMouseEvent *event);
            virtual void mouseReleaseEvent(QMouseEvent *event);

            EventController *evCont;

            std::vector<StableRegion*> *stRegions;
            StableRegion *primarySelectedRegion;
            StableRegion *secondarySelectedRegion;
            vector<XYPair*> highlightedRegions;
            bool mouseDown;

            QPoint dragLast;

            int offsetx;
            int offsety;
            int size;

            double absMaxSup;
            double absMinSup;
            double absMaxConf;
            double absMinConf;

            double minSupShown;
            double maxSupShown;
            double minConfShown;
            double maxConfShown;

            int uniqueMinRules;
            int uniqueMaxRules;
            int allMaxRules;
            int allMinRules;

            int uniqueMinRules_nr;
            int uniqueMaxRules_nr;
            int allMaxRules_nr;
            int allMinRules_nr;

            int redMinRules;
            int redMaxRules;
            int maxRules;
            int minRules;
            int curGranularity;

            bool hideCursor;

            ColorMap *colorMappings;
            ColorSelector *colorSelect;
            QColor secSelection;
            QColor firstSelection;

            QFrame *uniqueFrame;
            QScrollArea* Scroller;
            QRadioButton *uniqueButton;
            QRadioButton *nonUniqueButton;
            QButtonGroup *unBtnGrp;
            QRadioButton *gridDisabled;
            QRadioButton *gridActive;
            QRadioButton *skylineDisabled;
            QRadioButton *skylineActive;
            QRadioButton *redunInclude;
            QRadioButton *redunExclude;
            QLabel *granularityL;
            QLineEdit *setSkylineCardinality;
            QLineEdit *selectSupConf;

            QSlider *granularity;
        };
    }
}


#endif // PSPACEGRAPH_H
