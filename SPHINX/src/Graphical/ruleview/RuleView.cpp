#include <QRadioButton>
#include <QButtonGroup>

#include "graphical/ruleview/RuleView.hpp"
#include "graphical/ruleview/singlerule/SingleView.hpp"
#include "graphical/ruleview/doublerule/DoubleRuleTable.hpp"

using namespace PARASProgram::Graphical;

RuleView::RuleView(QWidget *parent, EventController *evCont, ColorMap *coloring) : QFrame(parent)
{
    this->colorMapping = coloring;
    singRT = new SingleView(this,evCont, coloring);
    singRT->setVisible(true);

    doubRT = new DoubleRuleTable(this,evCont);
    doubRT->setVisible(false);
}

void RuleView::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);

    QFont newFont = font();

    //Draw Scales
    QFontMetrics fm(newFont);
    setFont(newFont);


    painter.drawText(width() / 2 - fm.width("Rule Space View")/2, 30, "Rule Space View");
}

void RuleView::selectStableRegions(Nugget *primarySR, Nugget *secondarySR)
{
    doubRT->setVisible(false);
    singRT->setVisible(true);

    if(primarySR == NULL)
    {
        singRT->selectStableRegions(secondarySR, primarySR);
    }
    else
    {
        singRT->selectStableRegions(primarySR,secondarySR);
    }

    doubRT->selectStableRegions(primarySR,secondarySR);

    if(primarySR != NULL && secondarySR != NULL)
    {
        doubRT->setVisible(true);
        singRT->setVisible(false);
    }
}

void RuleView::applicationSizeChanged(int width, int height)
{
    int thisWidth = (width / 2 - 50);
    this->move(width/2+25,0);
    this->resize(thisWidth,height);

    singRT->applicationSizeChanged(thisWidth, height);
    doubRT->applicationSizeChanged(thisWidth,height );
}

void RuleView::setRuleMode(RuleMode ruleMode)
{
    singRT->setRuleMode(ruleMode);
    doubRT->setRuleMode(ruleMode);
}

void RuleView::updateIndexInfo(IndexUpdateEvent e)
{
    singRT->updateIndexInfo(e);
    doubRT->updateIndexInfo(e);
}

void RuleView::updateRedundancy(bool include)
{
    singRT->updateRedundancy(include);
    doubRT->updateRedundancy(include);
}

void RuleView::closeIndex()
{
    singRT->closeIndex();
    doubRT->closeIndex();
    singRT->setVisible(true);
    doubRT->setVisible(false);
}
