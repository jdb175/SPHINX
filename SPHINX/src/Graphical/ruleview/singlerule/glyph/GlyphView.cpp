#include "graphical/StableRegion.hpp"
#include <algorithm>
#include "graphical/ruleview/singlerule/glyph/GlyphView.hpp"
#include "graphical/ruleview/singlerule/RuleDetailView.hpp"

using namespace PARASProgram;
using namespace PARASProgram::Graphical;

GlyphView::GlyphView(QWidget *parent, ColorMap *coloring) : QWidget(parent)
{
    this->cMap = coloring;
    infoFrame = new QFrame(this);
    infoFrame->setAutoFillBackground(true);
    infoFrame->setFrameShape(QFrame::Box);
    infoFrame->setFrameShadow(QFrame::Sunken);

    detailView = new RuleDetailView(infoFrame, true);
    detailView->setVisible(true);
    gZoom = new GlyphZoom(infoFrame, cMap);
    gScroll = new QScrollArea(this);
    gDisplay = new GlyphDisplay(this, detailView, gZoom, cMap);
    gScroll->setWidget(gDisplay);

    displayRules = NULL;

    connectLabel = new QLabel(infoFrame);
    connectLabel->setText("<b>Connections</b>");
    connectShow = new QRadioButton(infoFrame);
    connectShow->setText("Show");
    connectHide = new QRadioButton(infoFrame);
    connectHide->setText("Hide");
    connectGroup = new QButtonGroup(infoFrame);
    connectGroup->addButton(connectHide);
    connectGroup->addButton(connectShow);
    connectGroup->connect(connectGroup, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(toggleConnections(QAbstractButton*)));
    connectShow->click();


    fillLabel = new QLabel(infoFrame);
    fillLabel->setText("<b>Fill</b>");
    fillEnabled = new QRadioButton(infoFrame);
    fillEnabled->setText("Enabled");
    fillDisabled = new QRadioButton(infoFrame);
    fillDisabled->setText("Disabled");
    fillGroup = new QButtonGroup(infoFrame);
    fillGroup->addButton(fillEnabled);
    fillGroup->addButton(fillDisabled);
    fillGroup->connect(fillGroup, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(toggleFill(QAbstractButton*)));
    fillDisabled->click();

    missingLabel = new QLabel(infoFrame);
    missingLabel->setText("<b>Missing</b>");
    missingEnabled = new QRadioButton(infoFrame);
    missingEnabled->setText("Shown");
    missingDisabled = new QRadioButton(infoFrame);
    missingDisabled->setText("Hidden");
    missingGroup = new QButtonGroup(infoFrame);
    missingGroup->addButton(missingEnabled);
    missingGroup->addButton(missingDisabled);
    missingGroup->connect(missingGroup, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(toggleMissing(QAbstractButton*)));
    missingDisabled->click();

    detailShow = new QRadioButton(detailView);
    detailShow->setText("Info");
    optionShow = new QRadioButton(detailView);
    optionShow->setText("Options");
    optionGroup = new QButtonGroup(detailView);
    optionGroup->addButton(detailShow);
    optionGroup->addButton(optionShow);
    optionGroup->connect(optionGroup, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(toggleDetailView(QAbstractButton*)));
    detailShow->click();

    grouped = false;
    this->setAutoFillBackground(true);
    detailOffset = 0;
}

void GlyphView::setGrouped(int clicked)
{
    grouped = (clicked == 0);
    this->gDisplay->toggleGrouped(grouped);
}

void GlyphView::toggleConnections(QAbstractButton* e)
{
    bool showConnections = e == connectShow;
    gDisplay->toggleConnections(showConnections);
    gZoom->toggleConnections(showConnections);
}

void GlyphView::toggleFill(QAbstractButton* e)
{
    bool fill = e == fillEnabled;
    gDisplay->toggleFill(fill);
    gZoom->toggleFill(fill);
}

void GlyphView::toggleMissing(QAbstractButton* e)
{
    bool missing = e == missingEnabled;
    gDisplay->toggleMissing(missing);
    gZoom->toggleMissing(missing);
}

void GlyphView::toggleDetailView(QAbstractButton *e)
{
    bool show = e == detailShow;
    detailView->showDetail(show);
    connectLabel->setVisible(!show);
    connectShow->setVisible(!show);
    connectHide->setVisible(!show);
    fillLabel->setVisible(!show);
    fillEnabled->setVisible(!show);
    fillDisabled->setVisible(!show);
    missingLabel->setVisible(!show);
    missingEnabled->setVisible(!show);
    missingDisabled->setVisible(!show);
}

void GlyphView::refreshGlyphs()
{
    glyphs = gDisplay->updateGlyphs(displayRules);
}

void GlyphView::updateRules(set<Rule*> *rules)
{
    displayRules = rules;
    this->refreshGlyphs();
}

void GlyphView::updateAttributes(vector<Attribute*> *attr)
{
    this->gDisplay->updateAttributeInfo(attr);
    this->gZoom->setAttributes(attr);
}


void GlyphView::applicationSizeChanged(int width, int height)
{
    this->move(0, 80);
    this->resize(width, height);

    gScroll->move(0,0);
    gScroll->resize(width,  height * 2 / 3 - 30);

    gDisplay->move(0,0);
    gDisplay->resize(width - 3, height * 2 / 3 - 33);

    infoFrame->move(0, gDisplay->y() + gDisplay->height() + 15);
    infoFrame->resize(width, 130);
    gZoom->move(0, 0);

    detailView->move(gZoom->x() + gZoom->width() + 5, 0);
    detailView->resize(infoFrame->width() - gZoom->width() - 3, 100);


    connectLabel->move(gZoom->x() + gZoom->width() + 5, 26);
    connectShow->move(connectLabel->x() + 2, 49);
    connectHide->move(connectShow->x(), connectShow->y() + 20);

    fillLabel->move(225, 26);
    fillEnabled->move(fillLabel->x() + 2, 49);
    fillDisabled->move(fillEnabled->x(), fillEnabled->y() + 20);
    missingLabel->move(310, 26);
    missingEnabled->move(missingLabel->x() + 2, 49);
    missingDisabled->move(missingEnabled->x(), missingEnabled->y() + 20);

    detailShow->move(detailOffset + 5, 3);
    optionShow->move(60, 3);

    this->refreshGlyphs();
}

void GlyphView::resetAll()
{
    displayRules = NULL;
    this->refreshGlyphs();
}



