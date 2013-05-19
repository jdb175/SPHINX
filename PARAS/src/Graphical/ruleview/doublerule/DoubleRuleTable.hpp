#ifndef DOUBLERULETABLE_HPP
#define DOUBLERULETABLE_HPP
#include <QWidget>
#include <QFrame>
#include <QLabel>
#include <QTableView>
#include <QStandardItemModel>
#include "graphical/EventController.hpp"
#include "paras/Attribute.hpp"

namespace PARASProgram
{
    namespace Graphical
    {
        class DoubleRuleTable : public QFrame, public EventListener
        {
            Q_OBJECT
            public:
                DoubleRuleTable(QWidget *par, EventController *evCont);

                //event listener
                void selectStableRegions(StableRegion *primarySR, StableRegion *secondarySR);
                void applicationSizeChanged(int width, int height);
                void setRuleMode(RuleMode ruleMode);
                void updateIndexInfo(IndexUpdateEvent e);
                void updateRedundancy(bool include);
                void closeIndex();

            private:
                void setupTable(QTableView *table);
                void populateTable(QStandardItemModel *model, set<Rule*> *rules);
                void resetAll();
                void normalizeColumnWidths(QTableView *tableWidget);

                bool redun;
                RuleMode mode;
                StableRegion *primarySelection;
                StableRegion *secondarySelection;

                QTableView *primaryTable;
                QTableView *secondaryTable;
                QTableView *interTable;
                QStandardItemModel *primaryModel;
                QStandardItemModel *secondaryModel;
                QStandardItemModel *interModel;

                QLabel *primaryLabel;
                QLabel *secondaryLabel;
                QLabel *interLabel;

                set<Rule*> *primaryDiff;
                set<Rule*> *secondaryDiff;
                set<Rule*> *intersection;
                vector<Attribute*> *attributes;
        };
    }
}


#endif // DOUBLERULETABLE_HPP
