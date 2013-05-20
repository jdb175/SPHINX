#ifndef RULEDETAILVIEW_HPP
#define RULEDETAILVIEW_HPP

#include <QWidget>
#include <QLabel>
#include <QTextEdit>
#include <vector>
#include "paras/Attribute.hpp"
#include "paras/Rule.hpp"

using namespace std;

namespace PARASProgram
{
    namespace Graphical
    {
        class RuleDetailView : public QWidget
        {
                Q_OBJECT
            public:
                explicit RuleDetailView(QWidget *parent = 0, bool showLegend = false);
                void setRuleCount(int ruleCount);
                void setSelectedRule(Rule* ru);
                void updateAttributes(vector<Attribute*> *attr);
                void resize(int w, int h);
                void showDetail(bool show);

            signals:

            public slots:

            private:
                QLabel *infoHeader;
                QTextEdit *infoArea;
                vector<Attribute*> *attributes;
                bool showLegend;
                bool showingDetail;


        };
    }
}

#endif // RULEDETAILVIEW_HPP
