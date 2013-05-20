#include "GlyphZoom.hpp"
#include "Glyph.hpp"
#include <QPaintEvent>
#include <QRect>

using namespace PARASProgram::Graphical;

GlyphZoom::GlyphZoom(QWidget *parent, ColorMap *coloring) : QFrame(parent)
{
    this->colorMapping = coloring;
    this->setAutoFillBackground(true);
    this->setFrameShape(QFrame::Box);
    this->setFrameShadow(QFrame::Sunken);
    this->resize(130,129);
    this->showConnections = true;
    this->drawnGlyph = new Glyph(colorMapping);
    this->drawnGlyph->setNaivePosition(0,0);
    this->drawnGlyph->setNaiveRadius(130/2);
    this->legendGlyph = new Glyph(colorMapping);
    this->legendGlyph->setNaivePosition(0,0);
    this->legendGlyph->setNaiveRadius(130/2);
    this->legendGlyph->setLegend(true);
    this->noGlyph = true;
    this->fill = false;
}

void GlyphZoom::setGlyph(Glyph* gl)
{
    noGlyph = false;
    if(gl == NULL)
    {
        noGlyph = true;
    }
    else
    {
        *drawnGlyph = *gl;
    }

    this->update();
}

void GlyphZoom::toggleConnections(bool connect)
{
    this->showConnections = connect;
    this->update();
}

void GlyphZoom::toggleFill(bool filling)
{
    this->fill = filling;
    this->update();
}

void GlyphZoom::toggleMissing(bool filling)
{
    this->missing = filling;
    this->update();
}

void GlyphZoom::setAttributes(vector<Attribute*> *attr)
{
    this->legendGlyph->setAttributes(attr);
}


void GlyphZoom::paintEvent(QPaintEvent *e)
{
    QPainter paint(this);
    paint.eraseRect(e->rect());
    paint.setRenderHint(QPainter::Antialiasing);
    paint.setPen(QColor(100,100,100,150));
    paint.drawRect(e->rect());
    if(drawnGlyph == NULL || noGlyph)
    {
        this->legendGlyph->draw(&paint, showConnections, fill, missing, false, true);
        paint.drawText(QPoint(75, 125), "Legend");
    }
    else
    {
        this->drawnGlyph->draw(&paint, showConnections,fill, missing, false, true);
    }
}
