#ifndef GLYPHDISPLAY_HPP
#define GLYPHDISPLAY_HPP

#include "graphical/ruleview/singlerule/glyph/Glyph.hpp"
#include "graphical/ruleview/singlerule/glyph/GlyphZoom.hpp"
#include "graphical/ruleview/singlerule/RuleDetailView.hpp"
#include "graphical/color/ColorMap.h"
#include <QWidget>
#include "graphical/StableRegion.hpp"
#include <set>

namespace PARASProgram
{
    namespace Graphical
    {
        class GlyphDisplay : public QWidget
        {
                Q_OBJECT
            public:
                GlyphDisplay(QWidget *parent, RuleDetailView *detail, GlyphZoom *zoom, ColorMap *coloring);
                vector<Glyph*>* updateGlyphs(set<Rule*> *displayRules);
                void updateAttributeInfo(vector<Attribute*> *attributeList);
                void toggleConnections(bool connect);
                void toggleFill(bool filling);
                void toggleMissing(bool filling);
                void toggleGrouped(bool group);
                void toggleLabels(bool label);
                void paintEvent(QPaintEvent *e);
                void mouseMoveEvent(QMouseEvent *e);
                void mouseReleaseEvent(QMouseEvent *e);
                void leaveEvent(QEvent *e);
                void updateSorting(bool sort);

            private:
                bool holdSelection;
                GlyphZoom *zoomWidget;
                RuleDetailView *detailView;
                bool showConnections;

                ColorMap *cMap;

                void recalculateGlyphSizes();

                void forceRepaint();
                int getTileSize(int width, int height, int tileCount);
                vector<Glyph*> *glyphs;
                vector<Attribute*> *attributesList;

                bool supSorting;
                bool fill;
                bool missing;
                bool grouped;
                bool labels;
                Glyph *lastHeld;

                double **mdsPlacement;

                int GLYPHS_PER_ROW;
        };
    }
}


#endif // GLYPHDISPLAY_HPP
