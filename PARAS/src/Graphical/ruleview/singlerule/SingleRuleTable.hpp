#ifndef SingleRULETABLE_H
#define SingleRULETABLE_H

#include <QWidget>
#include <QTableWidget>
#include <QStandardItemModel>
#include <set>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QTextEdit>
#include "paras/Rule.hpp"
#include "graphical/ruleview/singlerule/RuleDetailView.hpp"

using namespace std;

namespace PARASProgram
{
    namespace Graphical
    {
        class SingleRuleTable : public QFrame
        {
            Q_OBJECT
            public:
                SingleRuleTable(QWidget *parent);
                ~SingleRuleTable();

                int getColumnWidth(int col);
                int getColumnPosition(int col);

                //event listener
                void updateRules(set<Rule*> *newRules);
                void applicationSizeChanged(int width, int height);
                void updateAttributes(vector<Attribute*> *attr);

            public slots:
                void updateRuleSelection();

            private:
                QFrame *infoBox;
                RuleDetailView *detailView;

                QTableView *tableWidget;
                QStandardItemModel *tableModel;
                vector<Rule*> *rowRule;

                void buildTable();
                void buildInfoBox();

                void populateTable(set<Rule*> *rules);
                void resetAll();
                void resetTable();
                void normalizeColumnWidths();
        };
    }
}

#endif // RULETABLE_H
