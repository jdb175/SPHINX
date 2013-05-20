#ifndef GUI_HPP_
#define GUI_HPP_

#include <QMainWindow>
#include <QFileDialog>
#include <vector>
#include "paras/PARAS.hpp"
#include "graphical/PSpaceGraph.hpp"
#include "graphical/ruleview/RuleView.hpp"
#include "graphical/StableRegion.hpp"
#include "graphical/EventController.hpp"
#include "graphical/GUIEventObject.h"
#include "paras/Rule.hpp"

using namespace PARASProgram::Graphical;

namespace PARASProgram
{
    namespace Graphical
    {
        class GUI : public QMainWindow, public GUIEventObject
        {
            Q_OBJECT
            public:
                GUI();
                void start();
                void keyPressEvent(QKeyEvent *e);
                void keyReleaseEvent(QKeyEvent *e);

            public slots:
                void onQuit();
            private slots:
                void loadExistingIndex();
                void createPSpaceIndex();
                void saveIndex();
                void closeIndex();
            private:
                void loadConfigInfo();
                QMenuBar *buildMenuBar();
                PSpaceGraph *createPSpaceGraph();
                void updateForLoadedIndex(QString forFile);
                void resizeEvent(QResizeEvent *e);
                void updateGranularity(int g);
                void updateRedundancy(bool r);
                vector<XYPair*> *buildIntersections(vector<XYPair*> *pspacePoints);

                EventController *evCont;
                PARAS *pInstance;
                PSpaceGraph *graphInstance;
                RuleView *rulePanel;
                QWidget *centralWidget;
                QFrame *line;
                QAction *save;
                QAction *close;
                float truncateVal;
                bool redundancies;
                map<float, IndexUpdateEvent> *granularityIndexMap;
                map<XYPair*, set<Rule*>*> *stableRegionRulesALL;
                map<XYPair*, set<Rule*>*> *stableRegionRulesUNIQUE;
                map<XYPair*, set<Rule*>*> *stableRegionRulesALL_NR;
                map<XYPair*, set<Rule*>*> *stableRegionRulesUNIQUE_NR;
                vector< set<Rule*> > *uniqueRulesLists;

                ColorManager *colorManage;
                ColorSelector *colorSelect;
                ColorMap *colorMappings;

                string configPath;
                bool haveConfigFile;
        };
    }
}


#endif /* GUI_HPP_ */
