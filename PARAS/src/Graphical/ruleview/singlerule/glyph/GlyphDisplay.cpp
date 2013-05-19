#include "graphical/ruleview/singlerule/glyph/GlyphDisplay.hpp"

#define USE_PCA

#include <QPaintEvent>
#include <QToolTip>
#include <iostream>
#include <math.h>
#include <cmath>
#include <algorithm>
#include "util/Utility.hpp"
#include "util/mds/Multidimensional_scaling.h"
#include "util/pca/PCA.h"

using namespace PARASProgram::Graphical;

GlyphDisplay::GlyphDisplay(QWidget *parent, RuleDetailView* detail, GlyphZoom *zoom, ColorMap *coloring) : QWidget(parent)
{
    this->setMouseTracking(true);
    showConnections = true;
    zoomWidget = zoom;
    glyphs = new vector<Glyph*>();
    GLYPHS_PER_ROW = 10;
    cMap = coloring;
    supSorting = true;
    fill = false;
    missing = false;
    detailView = detail;
    holdSelection = false;
    grouped = false;
    lastHeld = NULL;

    mdsPlacement = NULL;
}

void GlyphDisplay::toggleConnections(bool connect)
{
    this->showConnections = connect;
    this->update();
}

void GlyphDisplay::toggleFill(bool filling)
{
    this->fill = filling;
    this->update();
}

void GlyphDisplay::toggleGrouped(bool group)
{
    this->grouped = group;
    this->update();
}


void GlyphDisplay::toggleMissing(bool filling)
{
    this->missing = filling;
    this->update();
}

void GlyphDisplay::updateAttributeInfo(vector<Attribute*> *attributeList)
{
    this->attributesList = attributeList;
    detailView->updateAttributes(attributeList);
}

void GlyphDisplay::updateSorting(bool sorting)
{
    this->supSorting = sorting;
    this->forceRepaint();
}

vector<Glyph*>* GlyphDisplay::updateGlyphs(set<Rule*> *displayRules)
{
    if(lastHeld) lastHeld->border = false;
    lastHeld = NULL;
    holdSelection = false;
    detailView->setSelectedRule(NULL);
    zoomWidget->setGlyph(NULL);

    for(vector<Glyph*>::iterator j = glyphs->begin(); j != glyphs->end(); ++j) delete *j;
    glyphs->clear();

    if(displayRules == NULL)
    {
        this->forceRepaint();
        detailView->setRuleCount(0);
    }
    else
    {
        //create a glyph for each rule
        for(set<Rule*>::iterator j = displayRules->begin(); j != displayRules->end(); ++j)
        {
            Rule *curRule = *j;
            Glyph *ruleGlyph = new Glyph(curRule, attributesList, cMap);
            glyphs->push_back(ruleGlyph);
        }

        int numGlyphs = glyphs->size();

        if(numGlyphs > 0)
        {
            //setup the clustering coordinates based on either PCA or MDS algorithm.
            double minX = std::numeric_limits<double>::max();
            double maxX = std::numeric_limits<double>::min();
            double minY = minX;
            double maxY = maxX;

            /*
             * Below is setup and running of PCA algorithm for layout.
             * This algorithm is much faster than MDS but results in some
             * strange layout artifacts.  For the interactivity of PARAS,
             * the increased performance is worth the drawbacks.
             */
#ifdef USE_PCA
            int dimSize = attributesList->size();
            int dataSize = numGlyphs;
            double **data = PCA::matrix(dataSize, dimSize);

            for(int i = 0; i < dataSize; ++i)
            {
                vector<double>* glyphData = (glyphs->at(i))->getData();

                for(int j = 0; j < dimSize; ++j)
                {
                    data[i][j] = glyphData->at(j);
                }
            }

            int prin_num = 2;
            if(dimSize < 2)
            {
                prin_num = dimSize;
            }
            else
            {
                prin_num = 2;
            }

            this->mdsPlacement = new double *[prin_num];
            for(int i = 0; i < prin_num; ++i)
            {
                this->mdsPlacement[i] = new double[dataSize];
            }
            double principal_max[prin_num];
            double principal_min[prin_num];

            for(int i = 0; i < prin_num; ++i)
            {
                principal_min[prin_num] = 0;
                principal_max[prin_num] = 1.0;
            }

            PCA::principal_Components_Analysis(dataSize, dimSize, data, prin_num, this->mdsPlacement, principal_max, principal_min);

            //normalize values & gather limits
            for(int i = 0; i < numGlyphs; ++i)
            {
                double mdsX = mdsPlacement[0][i];

                if(mdsX > maxX) maxX = mdsX;
                if(mdsX < minX) minX = mdsX;

                double mdsY = mdsPlacement[1][i];

                if(mdsY > maxY) maxY = mdsY;
                if(mdsY < minY) minY = mdsY;
            }

            maxY += 0.5;
            minY += 0.5;
            if(maxY > 1) maxY = 1;
            if(maxY < 0) maxY = 0;
            if(minY > 1) minY = 1;
            if(minY < 0) minY = 0;

            maxX += 0.5;
            minX += 0.5;
            if(maxX > 1) maxX = 1;
            if(maxX < 0) maxX = 0;
            if(minX > 1) minX = 1;
            if(minX < 0) minX = 0;

            double rangeY = maxY - minY;
            if(rangeY == 0) rangeY = 1;
            double rangeX = maxX - minX;
            if(rangeX == 0) rangeX = 1;

            for(int i = 0; i < numGlyphs; ++i)
            {
                double mdsX = mdsPlacement[0][i];
                double mdsY = mdsPlacement[1][i];

                mdsY += 0.5;
                if(mdsY < 0) mdsY = 0;
                if(mdsY > 1) mdsY = 1;
                mdsY = (mdsY - minY) / rangeY;

                mdsX += 0.5;
                if(mdsX < 0) mdsX = 0;
                if(mdsX > 1) mdsX = 1;
                mdsX = (mdsX - minX) / rangeX;

                mdsPlacement[0][i] = mdsX;
                mdsPlacement[1][i] = mdsY;
            }

            PCA::free_matrix(data, dataSize, dimSize);

            /*
             * Below is algorithm setup for MDS layout method.
             * This method results in arguable better placement but much
             * worse performance than PCA
             */
#else
            //setup mds placement.
            double **similarity_matrix = new double *[numGlyphs];

            for(int count = 0; count < numGlyphs; ++count)
                similarity_matrix[count] = new double[numGlyphs];

            for(int row = 0; row < numGlyphs; ++row)
            {
                double simval1 = glyphs->at(row)->simval;
                for(int col = 0; col < numGlyphs; ++col)
                {
                    double simval2 = glyphs->at(col)->simval;
                    double diff = simval1 - simval2;
                    similarity_matrix[row][col] = sqrt(diff * diff);
                }
            }

            if(mdsPlacement)
            {
                delete mdsPlacement[0];
                delete mdsPlacement[1];
                delete mdsPlacement;
            }

            mdsPlacement = new double*[2];
            mdsPlacement[0] = new double[numGlyphs];
            mdsPlacement[1] = new double[numGlyphs];

            multidimensional_scaling(numGlyphs, 2, similarity_matrix, mdsPlacement);

            //normalize values & gather limits
            for(int i = 0; i < numGlyphs; ++i)
            {
                double mdsX = mdsPlacement[0][i];
                double mdsY = mdsPlacement[1][i];

                if(std::isnan(mdsX)) mdsX = 0;
                if(std::isnan(mdsY)) mdsY = 0;

                if(mdsX > maxX) maxX = mdsX;
                if(mdsX < minX) minX = mdsX;

                if(mdsY > maxY) maxY = mdsY;
                if(mdsY < minY) minY = mdsY;

                mdsPlacement[0][i] = mdsX;
                mdsPlacement[1][i] = mdsY;
            }

            //cleanup
            for(int count = 0; count < numGlyphs; ++count)
                delete similarity_matrix[count];
            delete similarity_matrix;
#endif
        }
        this->forceRepaint();
    }
    detailView->setRuleCount(displayRules->size());

    return glyphs;
}

void GlyphDisplay::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);

    painter.eraseRect(e->rect());
    painter.setRenderHint(QPainter::Antialiasing);
    QFontMetrics fm(font());

    //if no rules, say so.
    if(glyphs == NULL || glyphs->size() == 0)
    {
        painter.drawText( width() / 2 - fm.width("No Rules to Display")/2, 40, "No Rules to Display");
        resize(width(), this->parentWidget()->height());
        return;
    }

    for(vector<Glyph*>::iterator j = glyphs->begin(); j != glyphs->end(); ++j)
    {
        Glyph *activeGlyph = *j;
        activeGlyph->draw(&painter, showConnections,fill, missing, grouped, false);
    }
}

void GlyphDisplay::forceRepaint()
{
    this->recalculateGlyphSizes();
    this->update();
}

void GlyphDisplay::recalculateGlyphSizes()
{
    int numGlyphs = glyphs->size();
    if(numGlyphs == 0) return;

    int glyphRadius = (getTileSize(width(), height(), numGlyphs)) / 2;

    int rows = floor(width()/(glyphRadius*2));
    int columns = floor(height()/(glyphRadius*2));

    double incrementx = std::max((double)(glyphRadius * 2), width()/(double)rows);
    double incrementy = std::max((double)(glyphRadius * 2), height()/(double)columns);

    double starting_x = 0;
    double starting_y = 0;

    double switch_line_at = width() - (glyphRadius * 2);
    double x = starting_x;
    double y = starting_y;

    int groupedRadius = std::max(std::min(width(), height()) / 20, glyphRadius);
    int count = 0;

    int mdsWidth = this->width() - (groupedRadius * 2);
    int mdsHeight = this->height() - (groupedRadius * 2);
    for(vector<Glyph*>::iterator j = glyphs->begin(); j != glyphs->end(); ++j)
    {
        Glyph *activeGlyph = *j;
        double mdsX = mdsPlacement[0][count];
        double mdsY = mdsPlacement[1][count];

        double groupedX = (mdsX * mdsWidth);
        double groupedY = (mdsY * mdsHeight);

        activeGlyph->setNaivePosition(x, y);
        activeGlyph->setGroupedPosition(groupedX, groupedY);
        activeGlyph->setNaiveRadius(glyphRadius);
        activeGlyph->setGroupedRadius(groupedRadius);

        count++;
        //move the pos for the next glyph.
        if((x + incrementx) >= switch_line_at)
        {
            y += incrementy;
            x = starting_x;
        }
        else
        {
            x += incrementx;
        }

    }
}

int GlyphDisplay::getTileSize(int width, int height, int tileCount)
{

    // quick bailout for invalid input
    if (width*height < tileCount) { return 0; }

    // come up with an initial guess
    double aspect = (double)height/width;
    double xf = sqrtf(tileCount/aspect);
    double yf = xf*aspect;
    int x = max(1.0, floor(xf));
    int y = max(1.0, floor(yf));
    int x_size = floor((double)width/x);
    int y_size = floor((double)height/y);
    int tileSize = min(x_size, y_size);

    // test our guess:
    x = floor((double)width/tileSize);
    y = floor((double)height/tileSize);
    if (x*y < tileCount) // we guessed too high
    {
        if (((x+1)*y < tileCount) && (x*(y+1) < tileCount))
        {
            // case 2: the upper bound is correct
            //         compute the tileSize that will
            //         result in (x+1)*(y+1) tiles
            x_size = floor((double)width/(x+1));
            y_size = floor((double)height/(y+1));
            tileSize = min(x_size, y_size);
        }
        else
        {
            // case 3: solve an equation to determine
            //         the final x and y dimensions
            //         and then compute the tileSize
            //         that results in those dimensions
            int test_x = ceil((double)tileCount/y);
            int test_y = ceil((double)tileCount/x);
            x_size = min(floor((double)width/test_x), floor((double)height/y));
            y_size = min(floor((double)width/x), floor((double)height/test_y));
            tileSize = max(x_size, y_size);
        }
    }

    return tileSize;
}

void GlyphDisplay::mouseMoveEvent(QMouseEvent *e)
{
    if(holdSelection) return;

    zoomWidget->setGlyph(NULL);
    detailView->setSelectedRule(NULL);

    if(glyphs->size() == 0) return;

    int x = e->pos().x();
    int y = e->pos().y();

    Glyph *curGlyph = NULL;
    for(vector<Glyph*>::iterator j = glyphs->begin(); j != glyphs->end(); ++j)
    {
        Glyph* gl = *j;
        if(gl->contains(x,y))
        {
            curGlyph = gl;
            break;
        }
    }

    if(curGlyph == NULL) return;

    zoomWidget->setGlyph(curGlyph);
    detailView->setSelectedRule(curGlyph->rule);

    e->accept();
}

void GlyphDisplay::mouseReleaseEvent(QMouseEvent *e)
{
    holdSelection = false;
    if(lastHeld) (lastHeld->border) = false;

    int x = e->pos().x();
    int y = e->pos().y();

    Glyph *curGlyph = NULL;
    for(vector<Glyph*>::iterator j = glyphs->begin(); j != glyphs->end(); ++j)
    {
        Glyph* gl = *j;
        if(gl->contains(x,y))
        {
            curGlyph = gl;
            break;
        }
    }

    holdSelection = curGlyph != NULL;

    if(holdSelection)
    {
        if(lastHeld == curGlyph)
        {
            holdSelection = false;
            lastHeld = NULL;
            zoomWidget->setGlyph(NULL);
            detailView->setSelectedRule(NULL);
        }
        else
        {
            lastHeld = curGlyph;
            curGlyph->border = true;
            detailView->setSelectedRule(curGlyph->rule);
            zoomWidget->setGlyph(curGlyph);
        }
    }
    else
    {
        zoomWidget->setGlyph(NULL);
        detailView->setSelectedRule(NULL);
    }


    update();
}

void GlyphDisplay::leaveEvent(QEvent *e)
{
    if(!holdSelection)
    {
        zoomWidget->setGlyph(NULL);
        detailView->setSelectedRule(NULL);
    }
}

