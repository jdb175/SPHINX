#include <QtGui>
#include <QtDebug>
#include <QWheelEvent>
#include <QFrame>
#include <algorithm>
#include <QRadioButton>
#include <QButtonGroup>
#include <QLabel>
#include <QSlider>
#include <QPushButton>
#include <QLineEdit>
#include <sstream>
#include <QScrollArea>
#include <QToolTip>
#include <limits>

#include "graphical/EventController.hpp"
#include "graphical/PSpaceGraph.hpp"
#include "graphical/StableRegion.hpp"
#include "graphical/color/ColorSelector.h"
#include "graphical/color/ColorManager.h"
#include "graphical/color/ColorMap.h"


using namespace PARASProgram::Graphical;

PSpaceGraph::PSpaceGraph(QWidget *parent, EventController *controller, ColorMap* cMap, ColorSelector *cSelect) : QWidget(parent)
{
    setMouseTracking(true);
    hideCursor = true;
    setAttribute(Qt::WA_Hover);
    this->evCont = controller;

    secondarySelect = false;
    stRegions = NULL;
    primarySelectedRegion = NULL;
    secondarySelectedRegion = NULL;
    offsetx = 75;
    offsety = 50;
    size = 0;

    ruleMode = RuleMode(UNIQUE);
    absMinSup = 0;
    absMaxSup = 1;
    absMinConf = 0;
    absMaxConf = 1;
    minSupShown = absMinSup;
    maxSupShown = absMaxSup;
    minConfShown = absMinConf;
    maxConfShown = absMaxConf;

    uniqueMinRules = 0;
    uniqueMaxRules = 0;
    allMaxRules = 0;
    allMinRules = 0;
    uniqueMinRules_nr = 0;
    uniqueMaxRules_nr = 0;
    allMaxRules_nr = 0;
    allMinRules_nr = 0;

    redMinRules = 0;
    redMaxRules = 0;

    minRules = 0;
    maxRules = 0;

    bgLines = true;
    skyline = true;
    skyLineCardinality = 0;
    useCardinality = false;
    includeRedundancies = false;

    colorSelect = cSelect;
    colorMappings = cMap;
    buildZoomFrame();
}

void PSpaceGraph::setConfig(bool bgLines, bool skyline, bool include, RuleMode ruleMode, int cardinality, int granularity)
{
    this->gridDisabled->click();
    this->skylineDisabled->click();
    if(bgLines) this->gridActive->click();
    if(skyline) this->skylineActive->click();
    this->setSkylineCardinality->setText(QString::number(cardinality));

    this->bgLines = bgLines;
    this->skyline = skyline;
    this->skyLineCardinality = cardinality;

    this->updateShownGranularity(granularity / TRUNCATE_INTERVAL);
    this->granularity->setValue(granularity / TRUNCATE_INTERVAL);

    this->redunExclude->click();
    if(include) this->redunInclude->click();

    this->uniqueButton->click();
    if(ruleMode == RuleMode(ALL)) this->nonUniqueButton->click();


    this->repaint();
}


PSpaceGraph::~PSpaceGraph()
{
    evCont->deregisterListener(this);
}

/**
 * Builds the zoom frame that resides below the graph.
 */
void PSpaceGraph::buildZoomFrame()
{

    uniqueFrame = new QFrame();

    QLabel *unique = new QLabel(uniqueFrame);
    unique->setText("<b>Rules</b>");
    unique->move(5,5);

    uniqueButton = new QRadioButton(uniqueFrame);
    uniqueButton->setText("Unique");
    uniqueButton->move(8,22);
    uniqueButton->setChecked(true);
    QObject::connect(uniqueButton, SIGNAL(clicked()), this, SLOT(slotRMode()));


    nonUniqueButton = new QRadioButton(uniqueFrame);
    nonUniqueButton->setText("All");
    nonUniqueButton->move(8,42);
    QObject::connect(nonUniqueButton, SIGNAL(clicked()), this, SLOT(slotRMode()));

    unBtnGrp = new QButtonGroup(uniqueFrame);
    unBtnGrp->addButton(uniqueButton);
    unBtnGrp->addButton(nonUniqueButton);

    QPushButton *colorMapButton = new QPushButton(uniqueFrame);
    colorMapButton->setText("Customize Colors");
    colorMapButton->move(8, 95);
    QObject::connect(colorMapButton, SIGNAL(clicked()), this, SLOT(colorChooser()));

    granularityL = new QLabel(uniqueFrame);
    granularityL->setText("<b>Granularity:</b> 50      ");
    granularityL->move(8,70);
    granularityL->resize(120,20);

    granularity = new QSlider(Qt::Horizontal, uniqueFrame);
    granularity->setTickPosition(QSlider::TicksBelow);
    granularity->setTickInterval(1);
    granularity->setSingleStep(1);
    granularity->setMaximum(MAX_TRUNCATE_VAL/TRUNCATE_INTERVAL);
    granularity->setMinimum(MIN_TRUNCATE_VAL/TRUNCATE_INTERVAL);
    granularity->move(130,72);
    granularity->resize(150,20);
    granularity->setValue(5);
    QObject::connect(granularity, SIGNAL(valueChanged(int)), this, SLOT(updateGranularity()));
    QObject::connect(granularity, SIGNAL(valueChanged(int)), this, SLOT(updateShownGranularity(int)));


    //Grid lines radio button control

    QLabel *gridLines = new QLabel(uniqueFrame);
    gridLines->setText("<b>Grid Lines</b>");
    gridLines->move(85,5);

    gridActive = new QRadioButton(uniqueFrame);
    gridActive->setText("Show");
    gridActive->move(85,22);
    gridActive->setChecked(true);
    QObject::connect(gridActive, SIGNAL(clicked()), this, SLOT(updateGridLines()));


    gridDisabled = new QRadioButton(uniqueFrame);
    gridDisabled->setText("Hide");
    gridDisabled->move(85,42);
    QObject::connect(gridDisabled, SIGNAL(clicked()), this, SLOT(updateGridLines()));

    QButtonGroup *gridBtnGroup = new QButtonGroup();
    gridBtnGroup->addButton(gridActive);
    gridBtnGroup->addButton(gridDisabled);

    //redundancies radio button control
    QLabel *redunLabel = new QLabel(uniqueFrame);
    redunLabel->setText("<b>Redundancies</b>");
    redunLabel->move(166,5);

    redunInclude = new QRadioButton(uniqueFrame);
    redunInclude->setText("Include");
    redunInclude->move(166,22);
    QObject::connect(redunInclude, SIGNAL(clicked()), this, SLOT(setRedundancy()));


    redunExclude = new QRadioButton(uniqueFrame);
    redunExclude->setText("Exclude");
    redunExclude->move(166,42);
    redunExclude->setChecked(true);
    QObject::connect(redunExclude, SIGNAL(clicked()), this, SLOT(setRedundancy()));

    QButtonGroup *redunBtnGrp = new QButtonGroup();
    redunBtnGrp->addButton(redunInclude);
    redunBtnGrp->addButton(redunExclude);

    //Skylines radio button control

    QLabel *skylines = new QLabel(uniqueFrame);
    skylines->setText("<b>Skyline</b>");
    skylines->move(275,5);

    skylineActive = new QRadioButton(uniqueFrame);
    skylineActive->setText("Show");
    skylineActive->move(275,22);
    skylineActive->setChecked(true);
    QObject::connect(skylineActive, SIGNAL(clicked()), this, SLOT(updateSkyline()));


    skylineDisabled = new QRadioButton(uniqueFrame);
    skylineDisabled->setText("Hide");
    skylineDisabled->move(275,42);
    QObject::connect(skylineDisabled, SIGNAL(clicked()), this, SLOT(updateSkyline()));

    QButtonGroup *skyBtnGroup = new QButtonGroup();
    skyBtnGroup->addButton(skylineActive);
    skyBtnGroup->addButton(skylineDisabled);

    QLabel *skylinesC = new QLabel(uniqueFrame);
    skylinesC->setText("<b>Skyline Cardinality</b>");
    skylinesC->move(350,5);

    setSkylineCardinality = new QLineEdit(uniqueFrame);
    setSkylineCardinality->move(350, 22);
    setSkylineCardinality->setEnabled(false);
    setSkylineCardinality->setText("");
    QRegExp rx1("[0-9]{0,6}");
    setSkylineCardinality->setValidator(new QRegExpValidator(rx1, uniqueFrame));
    QObject::connect(setSkylineCardinality, SIGNAL(editingFinished()), this, SLOT(updateSkyline()));
    setSkylineCardinality->clearFocus();


    QLabel *selection = new QLabel(uniqueFrame);
    selection->setText("<b>Selection</b>");
    selection->move(350,60);

    selectSupConf = new QLineEdit(uniqueFrame);
    selectSupConf->move(350, 80);
    selectSupConf->setEnabled(true);
    selectSupConf->setPlaceholderText("Support, Confidence");
    //QRegExp rx("(1|(0?.\\d{0,4}))");//, ((1(.(0{0,4})?)|(0?.\\d{0,4}))");

    QRegExp rx("((1(.0{0,6})?)|(0?.[0-9]{0,6})),( )?((1(.0{0,6})?)|(0?.[0-9]{0,6}))");
    selectSupConf->setValidator(new QRegExpValidator(rx, uniqueFrame));
    QObject::connect(selectSupConf, SIGNAL(editingFinished()), this, SLOT(SupConfInput()));
    uniqueFrame->clearFocus();

    Scroller = new QScrollArea(this);
    Scroller->setWidget(uniqueFrame);
    Scroller->setFrameShadow(QFrame::Sunken);
    Scroller->setFrameShape(QFrame::Box);
    Scroller->resize(size,offsety);
    Scroller->move(offsetx,offsety+size);
    Scroller->setAlignment(Qt::AlignHCenter);

}


/*
 * SLOTS
 */

/**
 * @brief PSpaceGraph::slotRMode updates the rule mode used by the stable regions.
 */
void PSpaceGraph::slotRMode()
{
    bool unique = uniqueButton->isChecked();
    if(unique)
    {
        evCont->setRuleMode(RuleMode(UNIQUE));
        setSkylineCardinality->setEnabled(false);
        setSkylineCardinality->setText("");
        useCardinality = false;
        updateSkyline();
    }
    else
    {
        evCont->setRuleMode(RuleMode(ALL));
        setSkylineCardinality->setEnabled(true);
        updateSkyline();
    }
}

/**
 * @brief PSpaceGraph::updateGranularity updates the granularity based on the slider.
 *  It fires off an event that updates the granularity for the entire program.
 *
 */
void PSpaceGraph::updateGranularity() {
    if(primarySelectedRegion != NULL)
    {
        primarySelectedRegion->deselect();
        primarySelectedRegion = NULL;
    }

    if(secondarySelectedRegion != NULL)
    {
        secondarySelectedRegion->deselect();
        secondarySelectedRegion = NULL;
    }
    evCont->selectStableRegions(NULL, NULL);
    int g = granularity->value()*TRUNCATE_INTERVAL;
    evCont->updateGranularity(g);
}

/**
 * @brief PSpaceGraph::updateShownGranularity updates the granularity value shown on the slider. it does not
 *update the program granularity or modify the data.
 *
 */
void PSpaceGraph::updateShownGranularity(int g) {
    curGranularity = g*TRUNCATE_INTERVAL;
    QString str(3);
    str.setNum(g*TRUNCATE_INTERVAL);
    granularityL->setText(QString("<b>Granularity:</b> ").append(str));
}

/**
 * @brief PSpaceGraph::colorChooser displays the color chooser.
 */
void PSpaceGraph::colorChooser()
{
    colorSelect->displayColorSelector(colorMappings);
}

/**
 * @brief PSpaceGraph::updateRedundancy updates the inclusion of redundant rules on the graph.
 */
void PSpaceGraph::setRedundancy()
{
    bool r = redunInclude->isChecked();
    evCont->updateRedundancy(r);
}

/**
 * @brief PSpaceGraph::updateGridLines updates the presence of the grid lines in the graph.
 */
void PSpaceGraph::updateGridLines()
{
    bgLines = gridActive->isChecked();
    repaint();
}

/**
 * @brief PSpaceGraph::updateSkyline updates the presence of skyline when selecting a region.
 */
void PSpaceGraph::updateSkyline()
{

    skyLineCardinality = atoi(setSkylineCardinality->text().toStdString().c_str());

    if(setSkylineCardinality->text() == "" || setSkylineCardinality->isReadOnly() || skyLineCardinality == 0){
        useCardinality = false;
    } else {
        useCardinality = true;
    }

    skyline = skylineActive->isChecked();
    if(!skyline) {
        setSkylineCardinality->setEnabled(false);
    } else if (ruleMode == RuleMode(ALL)){
        setSkylineCardinality->setEnabled(true);
    }

    repaint();
}


void PSpaceGraph::paintEvent(QPaintEvent *)
{
    //Handle cursor hiding
    QPoint mousePos = this->mapFromGlobal(QCursor::pos());
    if(mousePos.x() <= offsetx+size && mousePos.x() >= offsetx && mousePos.y() <= offsety+size && mousePos.y() >= offsety && ! colorSelect->isVisible() && hideCursor){
        qApp->setOverrideCursor( QCursor( Qt::BlankCursor ) );
    } else {
        qApp->setOverrideCursor( QCursor( Qt::ArrowCursor ) );
    }

    QPainter painter(this);
    //painter.setRenderHint(QPainter::Antialiasing);
    //normalizeZoom();
    //Draw Background
    painter.setPen(QColor("#000000"));
    painter.setBrush(QBrush("#ffffff"));
    painter.drawRect(offsetx, offsety, size, size);

    //Highlgiht regions containing dominating rules
    //highlightDominatingRegions();

    //Draw all stable regions
    if(stRegions != NULL) drawStableRegions(&painter);

    //Draw Hashmarks, lines, and all text
    drawGraph(&painter);

    //Draw skyline for selected region
    if(skyline)
        useCardinality ? drawCardinalitySkyline(&painter) : drawSkyline(&painter);

    //Draw cursor position lines
    drawCursorLines(&painter);
}

/**
 * Draw graph scales and hash marks
 */
void PSpaceGraph::drawGraph (QPainter *painter){

    painter->setPen(QColor("#000000"));
    QFont newFont = font();

    //Draw Scales
    setFont(newFont);
    QFontMetrics fm(newFont);
    int width_support=fm.width("Support");
    int width_confidence=fm.width("Confidence");
    int f_height = fm.height();

    painter->drawText(offsetx + size / 2 - fm.width("Paramater Space View")/2, offsety - 20, "Parameter Space View");

    painter->drawText(offsetx+size/2-width_support/2, offsety+size+f_height+30, "Support");

    painter->save();
    painter->rotate(270); // or 270
    painter->drawText(-(offsety+(size+width_confidence)/2),offsetx-50, "Confidence");
    painter->restore();

    //Set Font for Scale numbers
    newFont.setPointSize(10);

    double cscale = maxConfShown-minConfShown;
    double sscale = maxSupShown-minSupShown;

    double hashDiffc;
    double hashDiffs;
    QString str;


    //Draw little hashmarks (.01)
    hashDiffc = (double) size/cscale/100;
    hashDiffs = (double) size/sscale/100;

    //double lineScale = std::min(cscale,sscale);
    double lineFades = std::min((double)(.3-sscale)/.2,(double)1);
    double lineFadec = std::min((double)(.3-cscale)/.2,(double)1);

    for (int i=0; i <=100; i++)
    {
        if(i%10 == 0)
            continue;
        str = "0."+QString::number(i);

        //Support
        if(i >= minSupShown*100 && i <= maxSupShown*100){
            painter->drawLine(offsetx+((double)i-minSupShown*100)*hashDiffs, offsety+size-5, offsetx+((double)i-minSupShown*100)*hashDiffs, size+offsety+5);
            //draw background lines if scale small enough
            if(sscale < .3){
                painter->setPen(Qt::DashLine);
                if(bgLines) painter->drawLine(offsetx+((double)i-minSupShown*100)*hashDiffs, offsety+size, offsetx+((double)i-minSupShown*100)*hashDiffs, offsety);
                painter->setPen(QColor(0,0,0,255*lineFades));
                //Numbers
                painter->drawText(offsetx+((double)i-minSupShown*100)*hashDiffs-fm.width(str)/2, offsety+size+fm.height()+5, str);
                painter->setPen(QColor("#000000"));
            }
        }
        //Confidence
        if(i >= minConfShown*100 && i <= maxConfShown*100){
            painter->drawLine(offsetx-5, offsety+(maxConfShown*100-(double)i)*hashDiffc, offsetx+5, offsety+(maxConfShown*100-(double)i)*hashDiffc);
            //draw background lines if scale small enough
            if(cscale < .3){
                //Background Lines
                painter->setPen(Qt::DashLine);
                if(bgLines) painter->drawLine(offsetx, offsety+(maxConfShown*100-(double)i)*hashDiffc, offsetx+size, offsety+(maxConfShown*100-(double)i)*hashDiffc);
                painter->setPen(QColor(0,0,0,255*lineFadec));
                //Numbers
                painter->drawText(offsetx-fm.width(str)-5, offsety+(maxConfShown*100-(double)i)*hashDiffc+4, str);
                painter->setPen(QColor("#000000"));
            }
        }

    }
    //Now draw big hashmarks (.1)
    hashDiffc = (double) size/cscale/10;
    hashDiffs = (double) size/sscale/10;

    for (int i=0; i <=10; i++)
    {
        //Numbers
        if(i < 10){
            str = "0."+QString::number(i);
        } else {
            str = "1.0";
        }

        //Support
        if(i >= minSupShown*10 && i <= maxSupShown*10){
            //Hashmarks
            painter->drawLine(offsetx+((double)i-minSupShown*10)*hashDiffs, offsety+size-10, offsetx+((double)i-minSupShown*10)*hashDiffs, size+offsety+10);
            painter->setPen(Qt::DashLine);
            //Background lines
            if(bgLines) painter->drawLine(offsetx+((double)i-minSupShown*10)*hashDiffs, offsety+size, offsetx+((double)i-minSupShown*10)*hashDiffs, offsety);
            painter->setPen(QColor("#000000"));
            //Numbers
            painter->drawText(offsetx+((double)i-minSupShown*10)*hashDiffs-fm.width(str)/2, offsety+size+fm.height()+12, str);
        }

        //Confidence
        if(i >= minConfShown*10 && i <= maxConfShown*10){
            //Hasmarks
            painter->drawLine(offsetx-10, offsety+(maxConfShown*10-(double)i)*hashDiffc, offsetx+10, offsety+(maxConfShown*10-(double)i)*hashDiffc);
            painter->setPen(Qt::DashLine);
            //Background Lines
            if(bgLines) painter->drawLine(offsetx, offsety+(maxConfShown*10-(double)i)*hashDiffc, offsetx+size, offsety+(maxConfShown*10-(double)i)*hashDiffc);
            painter->setPen(QColor("#000000"));
            //Numbers
            painter->drawText(offsetx-fm.width(str)-12, offsety+(maxConfShown*10-(double)i)*hashDiffc+4, str);
        }
    }
}

/**
 * Highlights dominating regions for selected region
 */
void PSpaceGraph::highlightDominatingRegions(){
    return;
    //de-highlight previous region set
    for(vector<XYPair*>::iterator it = highlightedRegions.begin(); it != highlightedRegions.end(); it++){
        StableRegion* highlighted = getRegionFromPoint((*it)->getX(), (*it)->getY());
        if(highlighted != NULL && highlighted != primarySelectedRegion)
            highlighted->deselect();
    }
    highlightedRegions.clear();

    //Now highlight new ones
    //only if there is
    if((primarySelectedRegion == NULL) == (secondarySelectedRegion == NULL)) return;

    //to start we need to get the rules for this region
    StableRegion* region = primarySelectedRegion == NULL ? secondarySelectedRegion : primarySelectedRegion;
    set<Rule*> *rules = region->getRules(ruleMode, includeRedundancies);

    //pull all dominating points into a list
    for(set<Rule*>::iterator it = rules->begin(); it != rules->end(); it++){
        if((*it)->getDominantPointStrict() != NULL) {
            highlightedRegions.push_back((*it)->getDominantPointStrict());
        }
   }
    //Get distinct points and sort them
    std::sort(highlightedRegions.begin(), highlightedRegions.end());

    //now get the regions and highlight them
    for(vector<XYPair*>::iterator it = highlightedRegions.begin(); it != highlightedRegions.end(); it++){
        //get the stable region for this point
        //if it is different form the last stable region, then highlight it
        StableRegion* highlighted = getRegionFromPoint((*it)->getX(), (*it)->getY());
        if(highlighted != NULL)
            highlighted->highlight();
   }


    //might have to work in opposite direction
    //iterate through all regions
    //in each region check every rule
        //if that rule is dominated by the current region, then highlight it
}

/**
 * Draws skyline for the currently set cardinality
 */
void PSpaceGraph::drawCardinalitySkyline(QPainter *painter){
    if(!stRegions) return;

    QPen skyPen(QBrush(Qt::SolidPattern), 3);
    //skyPen.setColor(Qt::black);
    painter->setPen(skyPen);

    //Iterate through regions
    float sup = 0;
    QPoint curPoint(offsetx, offsety+size);
    QPoint newPoint;
    int lastBest = -1;

    while (true){
        float bestSup = 2; // the
        int minDif = -1;
        int best = -1;
        bool bottom = false;
        //find the closest point in terms of support, approaching from the left
        //iterate through each stable region
        for(int i = stRegions->size()-1; i != -1; --i) {
            /*Check its support
             *  If it has a support greater than or equal to the last highest, and
             *  confidence greater than or equal to our baseline, then it is our
             *  current best
             *
             *  If it has support greater than or equal to the curPoint, we break
             */
            float curSup = (*stRegions)[i]->support;
            int numRules = (*stRegions)[i]->getRules(RuleMode(ALL), includeRedundancies)->size();
            int curDif = std::abs(numRules - skyLineCardinality);


            if(curSup > sup){
                //qDebug() << i <<":: Cur Sup: " << curSup << "Best Sup: " << bestSup << "Num Rules: " << numRules << "Cur Dif: " << curDif << "Best Dif: " << minDif;
                if((curSup < bestSup ||  (curSup == bestSup && (curDif < minDif || minDif < 0))) && numRules >= skyLineCardinality){
                    bestSup = curSup;
                    minDif = curDif;
                    best = i;
                }
            } else {
                break;
            }
        }

        //Our point of reference is now the best sup we found
        sup = bestSup;

        if(best != -1){
            newPoint = getTopRightFromSupConf((*stRegions)[best]->support,(*stRegions)[best]->confidence);
        } else {
            //this is in case there is no valid region,then we drop down to 0 confidence
            newPoint = QPoint(curPoint.x(), offsety+size);
            bottom = true;
        }

        if(sup == 0)
            curPoint = QPoint(offsetx,newPoint.y());

        QPoint intersection = QPoint(curPoint.x(), newPoint.y());

        //Now draw line from current to intesection, and intersection to new

        //intersection to new (horizontal)
        if(newPoint.y() >= offsety && newPoint.y() <= offsety+size && newPoint.x() >= offsetx && intersection.x() <= offsetx+size){
            painter->drawLine(QPoint(std::max(intersection.x(), offsetx), intersection.y()), QPoint(std::min(newPoint.x(), offsetx+size), newPoint.y()));
        }

        //intersection to current (vertical)
        if(curPoint.x() != offsetx+size && curPoint.x() > offsetx && curPoint.x() <= offsetx+size && std::min(intersection.y(), curPoint.y()) <= offsety+size && std::max(intersection.y(), curPoint.y()) >= offsety){
            painter->drawLine(QPoint(intersection.x(), std::max(std::min(intersection.y(), curPoint.y()), offsety)), QPoint(intersection.x(), std::min(std::max(intersection.y(), curPoint.y()), offsety+size)));
        }
        //Now we set our current pont to the new point
        curPoint = newPoint;

        //if we found no match, then we must be done
        if(bestSup >= 1) {
            break;
        }
    }
}

/**
 * Draws skyline for the currently selected region
 */
void PSpaceGraph::drawSkyline(QPainter *painter){
    if((primarySelectedRegion == NULL && secondarySelectedRegion == NULL) || (primarySelectedRegion != NULL && secondarySelectedRegion != NULL))
        return;

    StableRegion *selectedRegion;
    if(primarySelectedRegion != NULL) {
        selectedRegion = primarySelectedRegion;
    } else {
        selectedRegion = secondarySelectedRegion;
    }

    QPen skyPen(QBrush(Qt::SolidPattern), 3);
    //skyPen.setColor(Qt::black);
    painter->setPen(skyPen);

    float sup = selectedRegion->support;
    float conf = selectedRegion->confidence;
    QPoint curPoint = getTopRightFromSupConf(selectedRegion->support, selectedRegion->confidence);
    QPoint newPoint;
    int lastBest = -1;

    //draw left\above
    while (true){
        float bestSup = -1; // the
        float bestConf = -1;
        int best = -1;
        //find the closest point in terms of support, approaching from the left
        //iterate through each stable region
        for(size_t i = 0; i != stRegions->size(); ++i) {
             /*Check its support
             *  If it has a support greater than or equal to the last highest, and
             *  confidence greater than or equal to our baseline, then it is our
             *  current best
             *
             *  If it has support greater than or equal to the curPoint, we break
             */
            float curSup = (*stRegions)[i]->support;
            float curConf = (*stRegions)[i]->confidence;
            if(curSup < sup) {
                if(curSup > bestSup && curConf >= conf){
                    bestSup = curSup;
                    bestConf = curConf;
                    best = i;
                }
            } else {
                //We have hit our cur point, so stop
                break;
            }
        }

        //if we found no match, then we must be done
        if(best == -1 || lastBest == best) {
            if(curPoint.x() >= offsetx && curPoint.y() >= offsety && curPoint.y() <= size+offsety)
                painter->drawLine(QPoint(offsetx, curPoint.y()), curPoint);
            break;
        }
        lastBest = best;

        //Our point of reference is now the best sup we found
        sup = bestSup;
       // conf = bestConf;
        newPoint = getTopRightFromSupConf((*stRegions)[best]->support,(*stRegions)[best]->confidence);
        QPoint intersection = QPoint(newPoint.x(), curPoint.y());

        //Now draw line from current to intesection, and intersection to new

        //intersection to new (vertical)
        if(newPoint.x() >= offsetx && newPoint.x() <= offsetx+size && std::max(intersection.y(), newPoint.y()) >= offsety && std::min(intersection.y(), newPoint.y()) <= offsety+size){
            painter->drawLine(QPoint(intersection.x(), std::max(std::min(intersection.y(), newPoint.y()), offsety)), QPoint(newPoint.x(), std::min(std::max(newPoint.y(), intersection.y()), offsety+size)));
        }
        //intersection to current (horizontal)
        if(curPoint.y() >= offsety && curPoint.y() <= offsety+size && curPoint.x() >= offsetx && intersection.x() <= offsetx+size){
            painter->drawLine(QPoint(std::max(intersection.x(), offsetx), intersection.y()), QPoint(std::min(curPoint.x(), offsetx+size), curPoint.y()));
        }

        //Now we set our current pont to the new point
        curPoint = newPoint;
    }

    //reset support and confidence values
    sup = selectedRegion->support;
    conf = selectedRegion->confidence;
    curPoint = getTopRightFromSupConf(selectedRegion->support, selectedRegion->confidence);

    //draw right\below
    while (true){
        bool bottom = false;
        float bestSup = 2; // the
        float bestConf = 2;
        int best = -1;
        //find the closest point in terms of support, approaching from the left
        //iterate through each stable region
        for(int i = stRegions->size()-1; i != -1; --i) {
             /*Check its support
             *  If it has a support greater than or equal to the last highest, and
             *  confidence greater than or equal to our baseline, then it is our
             *  current best
             *
             *  If it has support greater than or equal to the curPoint, we break
             */
            float curSup = (*stRegions)[i]->support;
            float curConf = (*stRegions)[i]->confidence;
            if(curSup > sup/* || (curSup == sup && curConf > conf)*/) {
                if(curSup <= bestSup && curConf <= conf){
                    bestSup = curSup;
                    bestConf = curConf;
                    best = i;
                }
            } else {
                //We have hit our cur point, so stop
                break;
            }
        }

        //if we found no match, then we must be done
        if(lastBest == best) {
            if(curPoint.y() < offsety+size)
                painter->drawLine(QPoint(offsetx+size,curPoint.y()), curPoint);
            break;
        }
        lastBest = best;
        bestConf = 2;


        //Our point of reference is now the new point we found
        sup = bestSup;

        if(best != -1){
            newPoint = getTopRightFromSupConf((*stRegions)[best]->support,(*stRegions)[best]->confidence);
        } else {
            //this is in case there is no valid region,then we drop down to 0 confidence
            newPoint = QPoint(curPoint.x(), offsety+size);
            bottom = true;
        }

        QPoint intersection = QPoint(curPoint.x(), newPoint.y());

        //Now draw line from current to intesection, and intersection to new

        //intersection to new (horizontal)
        if(newPoint.y() >= offsety && newPoint.y() <= offsety+size && newPoint.x() >= offsetx && intersection.x() <= offsetx+size){
            painter->drawLine(QPoint(std::max(intersection.x(), offsetx), intersection.y()), QPoint(std::min(newPoint.x(), offsetx+size), newPoint.y()));
        }

        //intersection to current (vertical)
        if((!bottom || curPoint.x() != offsetx+size) && curPoint.x() >= offsetx && curPoint.x() <= offsetx+size && std::min(intersection.y(), curPoint.y()) <= offsety+size && std::max(intersection.y(), curPoint.y()) >= offsety){
            painter->drawLine(QPoint(intersection.x(), std::max(std::min(intersection.y(), curPoint.y()), offsety)), QPoint(intersection.x(), std::min(std::max(intersection.y(), curPoint.y()), offsety+size)));
        }

        //Finally, we set our current pont to the new point
        curPoint = newPoint;
    }
}

/**
 * @brief PSpaceGraph::drawCursorLines draws horizontal and vertical lines crossing through the cursor
 * @param painter the painter for this paint event
 */
void PSpaceGraph::drawCursorLines(QPainter *painter){
    if(colorSelect->isVisible()) return;

    QPoint mousePos = this->mapFromGlobal(QCursor::pos());
    //Ensure that we are within the graph view
    if(mousePos.x() <= offsetx+size && mousePos.x() >= offsetx && mousePos.y() <= offsety+size && mousePos.y()>= offsety){
        //Initialize the pen
        //QPainterPath *textPath = new QPainterPath();
        QPen pen(QColor(0x44, 0x44, 0x44));
        pen.setWidth( 2 );
        pen.setStyle(Qt::DashLine);
        painter->setPen(pen);

        //now draw the cross lines
        painter->drawLine(QPoint(mousePos.x(),offsety), QPoint(mousePos.x(),offsety+size));
        painter->drawLine(QPoint(offsetx,mousePos.y()), QPoint(offsetx+size,mousePos.y()));


        //finally, draw the label
        double xPos = ((double)mousePos.x()- (double)offsetx)/(double)size;
        xPos = xPos*(maxSupShown-minSupShown)+minSupShown;
        double yPos = (double)1-((double)mousePos.y()- (double)offsety)/(double)size;
        yPos = yPos*(maxConfShown-minConfShown)+minConfShown;

        QFont newFont = font();
        newFont.setPointSize(12);
        painter->setFont(newFont);
        /*pen.setWidth(1);
        painter->setPen(pen);
        textPath->addText(QPoint(mousePos.x()+2, mousePos.y()-2), newFont ,(QString::number( xPos ) + ", " + QString::number( yPos )));
        pen.setColor(QColor(0xff, 0xff, 0xff, 0xa9));
        painter->setPen(pen);
        painter->drawText(QPoint(mousePos.x()+2, mousePos.y()-2) ,(QString::number( xPos ) + ", " + QString::number( yPos )));

        pen.setColor(QColor(0, 0, 0, 0xa9));
        painter->drawPath(*textPath);
        */
        painter->drawText(QPoint(mousePos.x()+2, mousePos.y()-2) ,(QString::number( xPos ) + ", " + QString::number( yPos )));
    }
}

/**
 * Returns the stable region under the given mouse position, if one exists.
 */
StableRegion* PSpaceGraph::getClickedRegion(QMouseEvent *event){
    if(stRegions == NULL) return NULL;
    //iterate through stable regions to find if we selected one
    double xPos = ((double)event->x()- (double)offsetx)/(double)size;
    xPos = xPos*(maxSupShown-minSupShown)+minSupShown;
    double yPos = (double)1-((double)event->y()- (double)offsety)/(double)size;
    yPos = yPos*(maxConfShown-minConfShown)+minConfShown;

    return getRegionFromPoint(xPos, yPos);
}

/**
 * Returns the stable region at the given point, if one exists.
 */
StableRegion* PSpaceGraph::getRegionFromPoint(double sup, double conf){
    if(stRegions == NULL) return NULL;

    if(sup < absMinSup || conf < absMinConf) return NULL;
    if(sup > absMaxSup || conf > absMaxConf) return NULL;
    if(sup < minSupShown || conf < minConfShown) return NULL;
    if(sup > maxSupShown || conf > maxConfShown) return NULL;

    StableRegion* sCur;

    for(std::vector<StableRegion*>::size_type i = 0; i != stRegions->size(); i++) {
        sCur = (*stRegions)[i];
        //qDebug() <<"Trying: "<< sCur->support << ", "<<sCur->confidence << "("<<sup<<", "<<conf<<")";
        //qDebug() << "    "<< (sCur->support >= sup) << ", " << (sCur->confidence >= conf);
        int numRules = sCur->getRules(ruleMode, includeRedundancies)->size();
        if(sCur->support >= sup && sCur->confidence >= conf && numRules >0){
          //  qDebug() << "FOUND IT";
            return sCur;
        }
    }
    return NULL;
}

/**
 * Update and draw all stable regions within the graph view.
 */
void PSpaceGraph::drawStableRegions(QPainter *painter){
    //Draw an example stable region
    for(int i = stRegions->size()-1; i != -1; i--) {
        updateStableRect((*stRegions)[i]);
        (*stRegions)[i]->draw(painter);
    }
}

/**
 * Pan the graph view based on the direction of mouse drag
 */
void PSpaceGraph::panDragged(QMouseEvent *event, QPoint dragLast){

    //How far have we dragged the graph?
    double scale = maxConfShown-minConfShown;
    double xdif = -(double)(event->pos().x()-dragLast.x())/450*scale;
    double ydif = (double)(event->pos().y()-dragLast.y())/450*scale;

    //Now pan the graph, without going past bounds
    if(minSupShown+xdif <absMinSup){
        xdif = absMinSup - minSupShown;
    }
    if(maxSupShown+xdif > absMaxSup){
        xdif = absMaxSup - maxSupShown;
    }
    minSupShown+=xdif;
    maxSupShown+=xdif;

    if(minConfShown+ydif <absMinConf){
        ydif = absMinConf - minConfShown;
    }
    if(maxConfShown+ydif > absMaxConf){
        ydif = absMaxConf - maxConfShown;
    }

    minConfShown+=ydif;
    maxConfShown+=ydif;
}

/**
 * Zoom the graph view, while remaining within bounds
 */
void PSpaceGraph::zoomScrolled(QWheelEvent* ev){
    double dif = (double)ev->delta()/2500;
    double xmouse = ev->globalX();
    double ymouse = ev->globalY();

    //check if we're within the bounds of the graph
    if(xmouse < offsetx || xmouse > size+offsetx || ymouse < offsety || ymouse > size+offsety)
        return;

    double centerx = (xmouse-offsetx)/(double)size;
    double centery = (ymouse-offsety)/(double)size;


    //now we want to zoom, ensuring our cursor remains at the same point
    //Confidence

    double minDif = (1-centery)*dif*(absMaxConf-absMinConf);
    double maxDif = (-centery)*dif*(absMaxConf-absMinConf);

    if(maxConfShown+ maxDif -(minConfShown+ minDif) > .1*(absMaxConf-absMinConf)){
        if(minConfShown+ minDif < absMinConf){
            maxDif -= minDif - (absMinConf - minConfShown);
            minDif = absMinConf - minConfShown;
        }

        if(maxConfShown+ maxDif > absMaxConf){
            minDif -= maxDif - (absMaxConf - maxConfShown);
            maxDif = absMaxConf - maxConfShown;
        }

        minDif = std::max((absMinConf - minConfShown), minDif);

        minConfShown+= minDif;
        maxConfShown+= maxDif;
    }


    //Support
    minDif = (centerx)*dif*(absMaxSup-absMinSup);
    maxDif = (centerx-1)*dif*(absMaxSup-absMinSup);

    if(maxSupShown+ maxDif -(minSupShown+ minDif) > .1*(absMaxSup-absMinSup)){
        if(minSupShown+ minDif < absMinSup){
            maxDif -= minDif - (absMinSup - minSupShown);
            minDif = absMinSup - minSupShown;
        }

        if(maxSupShown+ maxDif > absMaxSup){
            minDif -= maxDif - (absMaxSup - maxSupShown);
            maxDif = absMaxSup - maxSupShown;
        }

        minDif = std::max((absMinSup - minSupShown), minDif);

        minSupShown+= minDif;
        maxSupShown+= maxDif;
    }
}

/**
 * Ensure the pspace graph is within valid bounds
 */
void PSpaceGraph::normalizeZoom(){
    //Make sure we're within bounds
    if(minConfShown < absMinConf)
        minConfShown = absMinConf;
    if(maxConfShown > absMaxConf)
        maxConfShown = absMaxConf;

    if(minConfShown > maxConfShown-0.1*(absMaxConf-absMinConf))
        minConfShown = maxConfShown-0.1*(absMaxConf-absMinConf);

    if(minSupShown < absMinSup)
        minSupShown = absMinSup;
    if(maxSupShown > absMaxSup)
        maxSupShown = absMaxSup;
    if(minSupShown > maxSupShown-0.1*(absMaxSup-absMinSup))
        minSupShown = maxSupShown-0.1*(absMaxSup-absMinSup);
}

/**
 * @brief PSpaceGraph::getTopRightFromSupConf returns the relatively positioned QPoint for the given support and confidence
 * @param support
 * @param confidence
 * @param trim boolean for whether the values should be trimmed to fit within the graph
 * @return
 */
QPoint PSpaceGraph::getTopRightFromSupConf(double support, double confidence){
    //Adjust for zoom
    support = (support-minSupShown)/(maxSupShown-minSupShown);
    confidence = (confidence-minConfShown)/(maxConfShown-minConfShown);

    return QPoint(offsetx+(double)size*support, offsety+(double)size*(1-confidence));
}

/**
 * Update the drawn rect of the given stable region to match the current state of the graph
 */
void PSpaceGraph::updateStableRect(StableRegion *s)
{
    double sup = s->support;
    double conf = s->confidence;

    int numRules = s->getRules(ruleMode, includeRedundancies)->size();
    float colorProg;

    if(maxRules != minRules){
        colorProg = (float)(numRules- minRules)/(float)((maxRules + 2) - minRules);
        s->baseColor = QColor(colorMappings->getColor(colorProg));
    } else {
        s->baseColor = QColor(colorMappings->getColor(.5));
    }

    if(!s->selected)
        s->curColor = s->baseColor;

    int n;

    //Make sure the stable region is within view and has rules
    if(sup > minSupShown && conf > minConfShown && numRules > 0){
        //Adjust for zoom
        if(sup > maxSupShown){
            sup = maxSupShown;
        }
        sup = (sup-minSupShown)/(maxSupShown-minSupShown);
        if(conf > maxConfShown){
            conf = maxConfShown;
        }
        conf = (conf-minConfShown)/(maxConfShown-minConfShown);
        //now save the rectangle we'll have to draw
        s->setRect(QRect(QPoint(offsetx,offsety+(double)size*((double)1-conf)), QPoint(offsetx+(double)size*sup, offsety+(double)size)));
    } else {
        //Otherwise set it to an invalid rect which will not be drawn
        s->setRect(QRect(QPoint(-12,-12), QPoint(-13,-13)));
    }
}

void PSpaceGraph::resetGraph()
{
    primarySelectedRegion = NULL;
    secondarySelectedRegion = NULL;
    stRegions = NULL;

    absMinSup = 0;
    absMaxSup = 1;
    absMinConf = 0;
    absMaxConf = 1;
    minSupShown = absMinSup;
    maxSupShown = absMaxSup;
    minConfShown = absMinConf;
    maxConfShown = absMaxConf;

}

/*
 * Event listener functions
 */

void PSpaceGraph::selectStableRegions(StableRegion *primarySR, StableRegion *secondarySR)
{
    if(primarySelectedRegion != NULL) primarySelectedRegion->deselect();
    if(secondarySelectedRegion != NULL) secondarySelectedRegion->deselect();

    primarySelectedRegion = primarySR;
    secondarySelectedRegion = secondarySR;

    if(primarySelectedRegion != NULL) primarySelectedRegion->select(false);
    if(secondarySelectedRegion != NULL) secondarySelectedRegion->select(true);

    this->repaint();
}

void PSpaceGraph::applicationSizeChanged(int width, int height)
{
    this->move(50,0);
    this->resize(width/2,height);
    this->size = height * 2 /3 - 50;
    int ufsize = size;
    if(size > (width / 2 - offsetx - 100)) size = width / 2 - offsetx - 100;

    //uniqueFrame->move(offsetx, ufsize + offsety * 2 + 10);
    uniqueFrame->resize(500, 130);
    Scroller->resize(size,130);
    Scroller->move(offsetx, ufsize + offsety * 2 + 10);
}

void PSpaceGraph::setRuleMode(RuleMode ruleMode)
{
    this->ruleMode = ruleMode;
    switch(this->ruleMode)
    {
    case RuleMode(UNIQUE):
        if(this->includeRedundancies){
            this->minRules = this->uniqueMinRules;
            this->maxRules = this->uniqueMaxRules;
        } else {
            this->minRules = this->uniqueMinRules_nr;
            this->maxRules = this->uniqueMaxRules_nr;
        }
        break;
    case RuleMode(ALL):
        if(this->includeRedundancies){
            this->minRules = this->allMinRules;
            this->maxRules = this->allMaxRules;
        } else {
            this->minRules = this->allMinRules_nr;
            this->maxRules = this->allMaxRules_nr;
        }
        break;
    default:
        break;
    }
    repaint();
}

void PSpaceGraph::updateRedundancy(bool include)
{
    this->includeRedundancies = include;

    switch(this->ruleMode)
    {
    case RuleMode(UNIQUE):
        if(this->includeRedundancies){
            this->minRules = this->uniqueMinRules;
            this->maxRules = this->uniqueMaxRules;
        } else {
            this->minRules = this->uniqueMinRules_nr;
            this->maxRules = this->uniqueMaxRules_nr;
        }
        break;
    case RuleMode(ALL):
        if(this->includeRedundancies){
            this->minRules = this->allMinRules;
            this->maxRules = this->allMaxRules;
        } else {
            this->minRules = this->allMinRules_nr;
            this->maxRules = this->allMaxRules_nr;
        }
        break;
    default:
        break;
    }
    repaint();
}

void PSpaceGraph::updateIndexInfo(IndexUpdateEvent e)
{
    if((absMinSup == minSupShown && absMaxSup == maxSupShown) || (absMinConf == minConfShown && absMaxConf == minConfShown))
        resetGraph();

    absMinSup = e.supInterval.first * .99;
    absMaxSup = e.supInterval.second;
    absMinConf = e.confInterval.first * .99;
    absMaxConf = e.confInterval.second;

    stRegions = e.stableRegions;

    allMinRules = e.allInterval.first;
    allMaxRules = e.allInterval.second;
    uniqueMinRules = e.uniqueInterval.first;
    uniqueMaxRules = e.uniqueInterval.second;

    allMinRules_nr = e.allInterval_nr.first;
    allMaxRules_nr = e.allInterval_nr.second;
    uniqueMinRules_nr = e.uniqueInterval_nr.first;
    uniqueMaxRules_nr = e.uniqueInterval_nr.second;

    normalizeZoom();
    this->repaint();
    setRuleMode(this->ruleMode);

}

void PSpaceGraph::closeIndex()
{
    resetGraph();
}

/*
 * Mouse listener functions
 */

/**
 *When the mouse is pressed within the graph, begin watching for pan or selection
 */
void PSpaceGraph::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && event->x() > offsetx && event->x() < offsetx+ size && event->y() > offsety && event->y() < offsety+size){
        dragLast = event->pos();
        mouseDown = true;
    }
}

/**
 *If the user drags the mouse, pan the graph view. otherwise
 *show the
 */
void PSpaceGraph::mouseMoveEvent(QMouseEvent *event)
{
    if (mouseDown){
        panDragged(event, dragLast);
        this->repaint();
        if(dragLast != event->pos()) {
            mouseDragged = true;
            dragLast = event->pos();
        }
    }
    this->repaint();
}

/**
 *If the user releases the mouse without having dragged it, select the stable region they clicked
 *This is trying to be similar to a normal shift click, except with a max of two selected
 */
void PSpaceGraph::mouseReleaseEvent(QMouseEvent *event)
{
    selectSupConf->clearFocus();
    if (event->button() == Qt::LeftButton && mouseDown){
        if(!mouseDragged){
            StableRegion *clickedRegion = getClickedRegion(event);

            stringstream s("");
            if(clickedRegion != NULL){
                s << clickedRegion->support <<", " << clickedRegion->confidence;
            } else {
                s << "Support, Confidence";
            }

            if(secondarySelect)
            {
                //Shift-clicking selects additional region (up to two), trying to maintain the existing selection
                //Shift-clicking an already select region deselects it
                //if we have selected the primary region with shift click
                if(primarySelectedRegion == clickedRegion && clickedRegion != NULL) {
                    //then deselect it
                    primarySelectedRegion->deselect();
                    primarySelectedRegion = NULL;
                    evCont->selectStableRegions(NULL, secondarySelectedRegion);
                    selectSupConf->setText(QString::fromStdString(s.str()));
                    mouseDown = false;
                    return;
                }
                //If we have selected the secondary region with shift click
                if(secondarySelectedRegion == clickedRegion && clickedRegion != NULL){
                    //deselect it
                    secondarySelectedRegion->deselect();
                    secondarySelectedRegion = NULL;
                    evCont->selectStableRegions(primarySelectedRegion, NULL);
                    selectSupConf->setText(QString::fromStdString(s.str()));
                    mouseDown = false;
                    return;
                }

                //is there a primary selected region?
                if(primarySelectedRegion == NULL) {
                    // if there isn't just select as primary
                    evCont->selectStableRegions(clickedRegion, secondarySelectedRegion);
                    selectSupConf->setText("");
                } else {
                    //if there is, then
                    if(primarySelectedRegion == clickedRegion){
                        primarySelectedRegion->deselect();
                        evCont->selectStableRegions(NULL, secondarySelectedRegion);
                        selectSupConf->setText(QString::fromStdString(s.str()));
                    } else {
                        // then just select this one
                        evCont->selectStableRegions(primarySelectedRegion, clickedRegion);
                        selectSupConf->setText("");
                    }
                }
            }
            else
            {
                //Just left clicking always selects only the region you clicked (deselects all others)
                if(primarySelectedRegion != NULL){
                    primarySelectedRegion->deselect();
                }
                if(secondarySelectedRegion != NULL){
                    secondarySelectedRegion->deselect();
                    secondarySelectedRegion = NULL;
                }

                evCont->selectStableRegions(clickedRegion, NULL);
                selectSupConf->setText(QString::fromStdString(s.str()));
            }
        }
    }
    mouseDown = false;
    mouseDragged = false;
}

/**
 * The user has input a support and confidence in the input box, so we select that region
 */
void PSpaceGraph::SupConfInput(){
    QString text = selectSupConf->text();

    //pull support and confidence values from the string
    double support = text.split(',')[0].trimmed().toDouble();
    double confidence = text.split(',')[1].trimmed().toDouble();

    //Just to be doubly sure, don't allow invalid values
    confidence = std::max(std::min(absMaxConf,confidence), absMinConf);
    support = std::max(std::min(absMaxSup,support), absMinSup);

    qDebug() <<"Selecting: "<< support << ", "<<confidence;

    //Now deselect our previous selection
    if(primarySelectedRegion != NULL){
        primarySelectedRegion->deselect();
        primarySelectedRegion = NULL;
    }
    if(secondarySelectedRegion != NULL){
        secondarySelectedRegion->deselect();
        secondarySelectedRegion = NULL;
    }

    //Finally, select the region including our input point
    primarySelectedRegion = getRegionFromPoint(support-.0000001, confidence-.0000001);
    evCont->selectStableRegions(primarySelectedRegion, NULL);

}


/**
 * Zoom pspace graph based on the mouse wheel
 */
void PSpaceGraph::wheelEvent(QWheelEvent* ev){
    zoomScrolled(ev);
    this->repaint();
}

void PSpaceGraph::setCursorHiding(bool b){
    hideCursor = b;
}








