#ifndef GLYPHPANEL_HPP
#define GLYPHPANEL_HPP

#include <QWidget>
#include <QFrame>
#include <QScrollArea>
#include <QRadioButton>
#include <QButtonGroup>
#include <QLabel>
#include "graphical/color/ColorMap.h"
#include "graphical/ruleview/singlerule/glyph/GlyphDisplay.hpp"
#include "paras/Rule.hpp"
#include "graphical/ruleview/singlerule/glyph/Glyph.hpp"
#include "graphical/ruleview/singlerule/RuleDetailView.hpp"
#include "graphical/EventListener.hpp"

namespace SPHINXProgram
{
    namespace Graphical
    {
        class GlyphView : public QWidget
        {
                Q_OBJECT
            public:
                GlyphView(QWidget *parent, ColorMap *coloring);

                void updateRules(set<Rule*> *rules);
                void applicationSizeChanged(int width, int height);
                void updateAttributes(vector<Attribute*> *attr);

            private slots:
                void toggleConnections(QAbstractButton* e);
                void toggleFill(QAbstractButton* e);
                void toggleMissing(QAbstractButton* e);
                void toggleDetailView(QAbstractButton *e);
                void setGrouped(int clicked);
            private:
                void refreshGlyphs();
                void resetAll();

                ColorMap *cMap;
                GlyphZoom *gZoom;
                GlyphDisplay *gDisplay;
                RuleDetailView *detailView;

                QScrollArea *gScroll;

                QFrame *infoFrame;

                QLabel *connectLabel;
                QRadioButton *connectShow;
                QRadioButton *connectHide;
                QButtonGroup *connectGroup;

                QLabel *fillLabel;
                QRadioButton *fillEnabled;
                QRadioButton *fillDisabled;
                QButtonGroup *fillGroup;

                QLabel *missingLabel;
                QRadioButton *missingEnabled;
                QRadioButton *missingDisabled;
                QButtonGroup *missingGroup;

                QRadioButton *detailShow;
                QRadioButton *optionShow;
                QButtonGroup *optionGroup;

                vector<Attribute*> *attributeList;

                vector<Glyph*> *glyphs;
                set<Rule*> *displayRules;

                int detailOffset;

                bool grouped;
        };

    }
}


#endif // GLYPHPANEL_HPP
