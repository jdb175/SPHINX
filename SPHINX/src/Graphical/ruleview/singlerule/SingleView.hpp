#ifndef SINGLEVIEW_HPP
#define SINGLEVIEW_HPP

#include <QWidget>
#include <QPushButton>
#include <QStringListModel>
#include "graphical/EventListener.hpp"
#include "graphical/EventController.hpp"
#include "graphical/ruleview/singlerule/SingleRuleTable.hpp"
#include "graphical/ruleview/singlerule/glyph/GlyphView.hpp"
#include "graphical/ruleview/singlerule/FilterEditor.hpp"
#include "graphical/ruleview/singlerule/FilterCompleter.hpp"


namespace PARASProgram
{
    namespace Graphical
    {
        class SingleView : public QWidget, public EventListener
        {
                Q_OBJECT
            public:
                SingleView(QWidget *parent = 0, EventController *evCont = 0, ColorMap *colorMapping = 0);

                //event listener
                void selectStableRegions(StableRegion *primarySR, StableRegion *secondarySR);
                void applicationSizeChanged(int width, int height);
                void setRuleMode(RuleMode ruleMode);
                void updateIndexInfo(IndexUpdateEvent e);
                void updateRedundancy(bool include);
                void closeIndex();

            signals:

            public slots:
                void resetFilter();
                void filterResults();

            private slots:
                void changeViewMode(QAbstractButton* clickedButton);

            private:
                void buildFilterBox();
                void refreshRules();

                set<Rule*> *currentRules;

                StableRegion *selectedRegion;
                RuleMode ruleMode;
                bool includeRedundancies;

                EventController *evCont;

                FilterEditor *anteFilter;
                QString antePlaceHolder;
                QPushButton *anteFilterLabel;

                FilterEditor *consFilter;
                QString consPlaceHolder;
                QPushButton *consFilterLabel;

                FilterCompleter *consCompleter;
                FilterCompleter *anteCompleter;

                QStringListModel *anteModel;
                QStringListModel *consModel;


                QRadioButton *glyphToggle;
                QRadioButton *tableToggle;
                QButtonGroup *butGroup;
                QLabel *viewTypeLabel;

                QLabel *sortLabel;
                QRadioButton *supSort;
                QRadioButton *confSort;
                QButtonGroup *sortGroup;

                SingleRuleTable *singRT;
                GlyphView *glyph;

                vector<Attribute*> *attributes;
        };
    }
}


#endif // SINGLEVIEW_HPP
