#ifndef GLYPHZOOM_HPP
#define GLYPHZOOM_HPP

#include <QFrame>
#include <QLabel>
#include "graphical/ruleview/singlerule/glyph/Glyph.hpp"
#include "graphical/color/ColorMap.h"

namespace PARASProgram
{
    namespace Graphical
    {
        class GlyphZoom : public QFrame
        {
                Q_OBJECT
            public:
                GlyphZoom(QWidget *parent, ColorMap *coloring);
                void setGlyph(Glyph* gl);
                void paintEvent(QPaintEvent *e);
                void toggleConnections(bool connect);
                void toggleFill(bool fill);
                void toggleMissing(bool fill);
                void setAttributes(vector<Attribute*> *attr);
            private:
                ColorMap *colorMapping;
                Glyph *drawnGlyph;
                Glyph *legendGlyph;
                bool noGlyph;
                bool showConnections;
                bool fill;
                bool missing;
        };
    }
}


#endif // GLYPHZOOM_HPP
