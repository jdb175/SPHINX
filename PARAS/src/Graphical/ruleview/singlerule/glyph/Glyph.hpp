#ifndef GLYPH_HPP
#define GLYPH_HPP

#include <vector>
#include <QPainter>
#include <QLine>
#include "graphical/color/ColorMap.h"
#include "paras/Rule.hpp"
#include "paras/Attribute.hpp"

using namespace std;
namespace PARASProgram
{
    namespace Graphical
    {
        class Glyph
        {
            public:
                Glyph(Rule *rule, vector<Attribute*> *attributes, ColorMap *coloring);
                Glyph(ColorMap *coloring);
                ~Glyph();
                Glyph& operator=(const Glyph &source);

                void setLegend(bool leg);
                void setNaivePosition(double x, double y);
                void setGroupedPosition(double x, double y);
                void setNaiveRadius(double radius);
                void setGroupedRadius(double radius);
                void setAttributes(vector<Attribute*> *attr);

                vector<double>* getData();

                void draw(QPainter *painter, bool connections, bool fillingShape, bool missing, bool grouped, bool showLabels);

                bool contains(double x, double y);

                Rule *rule;
                double simval;
                bool border;

            protected:
                vector<Attribute*> *attributesList;

            private:
                void drawRays(QPainter *painter, vector<int> *indices, double deg_increment, bool showLabels);
                bool intersectsWithPreviouslyDrawn(QLineF toCheck);
                void buildGlyph();
                void buildLegend();

                vector<QPoint> *arcPoints;
                vector<double> *lengthArray;
                vector<int> *consequentIndices;
                vector<int> *antecedentIndices;
                vector<int> *missingIndices;
                vector<QLineF> *drawnRays;
                QPoint originPoint;
                ColorMap *cMap;
                bool legend;
                double x;
                double gx;
                double drawnX;
                double y;
                double gy;
                double drawnY;
                double radius;
                double gRadius;
                double drawnRadius;

        };
    }
}


#endif // GLYPH_HPP
