#include "RuleDetailView.hpp"
#include "paras/Rule.hpp"
#include "paras/Attribute.hpp"

using namespace SPHINXProgram::Graphical;

RuleDetailView::RuleDetailView(QWidget *parent, bool showLegend) : QWidget(parent)
{
    infoHeader = new QLabel(this);
    infoHeader->setAlignment(Qt::AlignRight);
    infoHeader->setText("No Stable Region Selected.");
    infoArea = new QTextEdit(this);
    infoArea->resize(350,80);
    infoArea->setReadOnly(true);
    infoArea->setWordWrapMode(QTextOption::WordWrap);
    infoArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    infoArea->move(2,27);
    infoHeader->resize(168, 20);
    this->setVisible(true);
    this->showLegend = showLegend;
    this->showingDetail = true;
    this->attributes = NULL;
    setRuleCount(0);
    this->resize(600,100);
}

void RuleDetailView::setSelectedRule(Rule* ru)
{
    if(attributes == NULL) return;
    if(ru == NULL && !showLegend)
    {
        infoArea->setVisible(false);
    }
    else
    {

        QString detail("");
        int count = 1;
        for(vector<Attribute*>::iterator i = attributes->begin(); i != attributes->end(); ++i)
        {
            Attribute* attr = *i;

            if(attr == NULL) continue;

            if(ru == NULL && showLegend)
            {
                if(attr->getAttributeName() == NULL) continue;
                QString attrName = attr->getAttributeName()->c_str();

                detail += "(" + QString::number(count) + ") " + attrName + "<br>";
            }
            else
            {
                QString xString = (attr->detailText(ru->getX(), count)).c_str();
                QString yString = (attr->detailText(ru->getY(), count)).c_str();
                detail += xString;
                detail += yString;
            }

            ++count;
        }

        if(ru != NULL)
        {
            QString sup("<br>  <b>Support: </b>");
            sup.append(QString::number(ru->getSupport()));
            QString conf("<br>  <b>Confidence: </b>");
            conf.append(QString::number(ru->getConfidence()));

            detail.append(sup);
            detail.append(conf);
        }

        infoArea->setText(detail);

        infoArea->setVisible(true && showingDetail);
    }
}

void RuleDetailView::resize(int w, int h)
{
    infoArea->resize(w - 10, h - 20);

    infoHeader->move(infoArea->x() + infoArea->width() - infoHeader->width(), 5);
    QWidget::resize(w,h);
}

void RuleDetailView::showDetail(bool show)
{
    this->showingDetail = show;
    infoArea->setVisible(showingDetail);
}

void RuleDetailView::setRuleCount(int ruleCount)
{
    if(ruleCount == 0)
    {
        infoHeader->setText("No Stable Region Selected.");
        if(!showLegend) infoArea->setVisible(false);
    }
    else
    {
        QString ruleStr("<b>Total Rules: </b>");
        ruleStr += QString::number(ruleCount);
        infoHeader->setText(ruleStr);
    }
    resize(width(), height());
}

void RuleDetailView::updateAttributes(vector<Attribute*> *attr)
{
    this->attributes = attr;
    this->setSelectedRule(NULL);
}
