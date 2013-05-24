#include "SingleView.hpp"
#include "paras/Attribute.hpp"
#include "graphical/EventController.hpp"
#include "graphical/ruleview/singlerule/FilterEditor.hpp"
#include "graphical/ruleview/singlerule/FilterCompleter.hpp"
#include <iostream>

using namespace SPHINXProgram::Graphical;

SingleView::SingleView(QWidget *parent, EventController *evCont, ColorMap *colorMapping) : QWidget(parent)
{
    currentRules = new set<Rule*>();
    this->evCont = evCont;
    ruleMode = RuleMode(UNIQUE);
    selectedRegion = NULL;
    includeRedundancies = false;
    singRT = new SingleRuleTable(this);
    singRT->setVisible(true);

    glyph = new GlyphView(this, colorMapping);
    glyph->setVisible(false);

    butGroup = new QButtonGroup();
    glyphToggle = new QRadioButton(this);
    glyphToggle->setText("Glyph");
    tableToggle = new QRadioButton(this);
    tableToggle->setText("Table");
    tableToggle->setChecked(true);
    viewTypeLabel = new QLabel(this);
    viewTypeLabel->setText("<b>View Mode:</b> ");
    butGroup->addButton(glyphToggle);
    butGroup->addButton(tableToggle);
    QObject::connect(butGroup, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(changeViewMode(QAbstractButton*)));
    buildFilterBox();

    supSort = new QRadioButton(this);
    supSort->setText("True");
    supSort->resize(65,20);
    confSort = new QRadioButton(this);
    confSort->setText("False");
    sortLabel = new QLabel(this);
    sortLabel->resize(100,20);
    sortLabel->setText("Group Glyphs:");
    sortGroup = new QButtonGroup();
    sortGroup->addButton(supSort,0);
    sortGroup->addButton(confSort,1);
    sortGroup->connect(sortGroup, SIGNAL(buttonClicked(int)), glyph, SLOT(setGrouped(int)));
    confSort->click();
    supSort->setVisible(false);
    confSort->setVisible(false);
    sortLabel->setVisible(false);
}

void SingleView::changeViewMode(QAbstractButton* clickedButton)
{
    bool view = clickedButton == glyphToggle;
    singRT->setVisible(!view);
    glyph->setVisible(view);
    supSort->setVisible(view);
    confSort->setVisible(view);
    sortLabel->setVisible(view);

    anteFilter->clearFocus();
    consFilter->clearFocus();
}

/**
 * @brief SingleView::buildFilterBox creates the filter boxes that sit over the header.
 */
void SingleView::buildFilterBox()
{
    antePlaceHolder = QString("Filter Antecedent.");
    consPlaceHolder = QString("Filter Consequent.");

    //setup the antecedent filter.
    anteFilter = new FilterEditor(this);
    anteFilter->setPlaceholderText(QString(antePlaceHolder));
    QObject::connect(anteFilter, SIGNAL(editingFinished()), this, SLOT(filterResults()));

    anteFilterLabel = new QPushButton(this);
    anteFilterLabel->setFlat(true);
    anteFilterLabel->setVisible(false);
    QObject::connect(anteFilterLabel, SIGNAL(pressed()), this, SLOT(resetFilter()));

    anteCompleter = new FilterCompleter();
    anteModel = new QStringListModel(this);

    anteFilter->setCompleter(anteCompleter);
    anteFilter->setVisible(true);
    //setup the consequent filter.
    consFilter = new FilterEditor(this);
    consFilter->setPlaceholderText(QString(consPlaceHolder));
    QObject::connect(consFilter, SIGNAL(editingFinished()), this, SLOT(filterResults()));

    consFilterLabel = new QPushButton(this);
    consFilterLabel->setFlat(true);
    consFilterLabel->setVisible(false);
    QObject::connect(consFilterLabel, SIGNAL(pressed()), this, SLOT(resetFilter()));

    consCompleter = new FilterCompleter();
    consModel = new QStringListModel(this);

    consFilter->setCompleter(consCompleter);
}

/**
 * @brief SingleView::filterResults
 */
void SingleView::filterResults()
{
    if(anteFilter->text().trimmed().length() > 0)
    {
        anteFilterLabel->setText(anteFilter->text());
        anteFilter->setupFilter();
        anteFilter->clear();
        anteFilter->setPlaceholderText(QString());
        anteFilter->setEnabled(false);

        anteFilterLabel->setDown(false);
        anteFilterLabel->setVisible(true);
    }

    if(consFilter->text().trimmed().length() > 0)
    {
        consFilterLabel->setText(consFilter->text());
        consFilter->setupFilter();
        consFilter->clear();
        consFilter->setPlaceholderText(QString());
        consFilter->setEnabled(false);

        consFilterLabel->setDown(false);
        consFilterLabel->setVisible(true);
    }

    if(selectedRegion != NULL) refreshRules();
    anteFilter->clearFocus();
    consFilter->clearFocus();
}

/**
 * @brief SingleView::refreshRules synchronizes the rules between the glyph and table view and filters them based on
 * the current rule mode, redundancy mode and filtering options.
 */
void SingleView::refreshRules()
{
    this->currentRules->clear();
    set<Rule*> *rulesFromStableRegion;

    if(this->selectedRegion != NULL)
    {
        rulesFromStableRegion = selectedRegion->getRules(ruleMode, includeRedundancies);
    }
    else
    {
        rulesFromStableRegion = currentRules;
    }

    set<vector<string> > antecedentSet;
    set<vector<string> > consequentSet;

    for(set<Rule*>::iterator iter = rulesFromStableRegion->begin(); iter != rulesFromStableRegion->end(); ++iter)
    {
        Rule* ru = *iter;
        vector<string*> *antecedent = ru->getX();
        vector<string*> *consequent = ru->getY();

        if(ru->displayAnte == NULL || ru->displayCons == NULL)
        {
            vector<string*> *toBeNamedAnte = new vector<string*>();
            vector<string*> *toBeNamedCons = new vector<string*>();

            for(vector<Attribute*>::iterator a = this->attributes->begin(); a != this->attributes->end(); ++a)
            {
                Attribute* curr = *a;
                curr->convertToNamed(antecedent, toBeNamedAnte);
                curr->convertToNamed(consequent, toBeNamedCons);
            }

            ru->displayAnte = toBeNamedAnte;
            ru->displayCons = toBeNamedCons;
        }

        if(ru->displayAnte->size() > 0) antecedent = ru->displayAnte;
        if(ru->displayCons->size() > 0) consequent = ru->displayCons;

        bool anteFiltered = anteFilter->isValid(antecedent);
        bool consFiltered = consFilter->isValid(consequent);
        bool valid = anteFiltered && consFiltered;

        if(valid)
        {
            this->currentRules->insert(ru);

            vector<string> antecedentVector;
            vector<string> consequentVector;
            for(vector<string*>::iterator j = antecedent->begin(); j != antecedent->end(); ++j) antecedentVector.push_back(*(*j));
            for(vector<string*>::iterator j = consequent->begin(); j != consequent->end(); ++j) consequentVector.push_back(*(*j));
            antecedentSet.insert(antecedentVector);
            consequentSet.insert(consequentVector);
        }
    }

    this->anteCompleter->updateModel(antecedentSet);
    this->consCompleter->updateModel(consequentSet);

    this->singRT->updateRules(currentRules);
    this->glyph->updateRules(currentRules);
}

void SingleView::resetFilter()
{
    if(anteFilterLabel->isDown())
    {
        anteFilterLabel->setVisible(false);
        anteFilter->resetFilter();
        anteFilter->clear();
        anteFilter->setEnabled(true);
        anteFilter->setPlaceholderText(antePlaceHolder);
    }

    if(consFilterLabel->isDown())
    {
        consFilterLabel->setVisible(false);
        consFilter->resetFilter();
        consFilter->clear();
        consFilter->setEnabled(true);
        consFilter->setPlaceholderText(consPlaceHolder);
    }

    this->refreshRules();
}


void SingleView::selectStableRegions(Nugget *primarySR, Nugget *secondarySR)
{
    this->selectedRegion = primarySR;
    this->refreshRules();
}

void SingleView::applicationSizeChanged(int width, int height)
{
    this->resize(width, height);
    this->move(0,0);
    glyph->applicationSizeChanged(width, height);
    singRT->applicationSizeChanged(width,height);

    //normalize filters
    anteFilter->resize(singRT->getColumnWidth(0) - 2, 24);
    anteFilter->move(singRT->getColumnPosition(0), 50);
    anteFilterLabel->resize(singRT->getColumnWidth(0) - 2, 24);
    anteFilterLabel->move(singRT->getColumnPosition(0), 50);

    consFilter->resize(singRT->getColumnWidth(1) - 4, 24);
    consFilter->move(singRT->getColumnPosition(1) + 2, 50);
    consFilterLabel->resize(singRT->getColumnWidth(1) - 4, 24);
    consFilterLabel->move(singRT->getColumnPosition(1) + 2, 50);

    sortLabel->move(consFilter->x() + consFilter->width() + 5, 50);
    supSort->move(sortLabel->x() + sortLabel->width() - 5, 50);
    confSort->move(supSort->x() + supSort->width() - 10, 50);

    tableToggle->move(width - tableToggle->width(),height * 2/3 + 185 - tableToggle->height());
    glyphToggle->move(tableToggle->x() - tableToggle->width() - 5, height * 2/3 + 185 - glyphToggle->height());
    viewTypeLabel->move(glyphToggle->x() - glyphToggle->width() - 20, height * 2/3 + 185 - viewTypeLabel->height() - 1);
}

void SingleView::setRuleMode(RuleMode ruleMode)
{
    this->ruleMode = ruleMode;
    refreshRules();
}

void SingleView::updateIndexInfo(IndexUpdateEvent e)
{
    this->selectedRegion = NULL;
    this->glyph->updateAttributes(e.attributes);
    this->singRT->updateAttributes(e.attributes);
    this->attributes = e.attributes;
    refreshRules();
}

void SingleView::updateRedundancy(bool include)
{
    this->includeRedundancies = include;
    refreshRules();
}

void SingleView::closeIndex()
{
    this->selectedRegion = NULL;
    refreshRules();
}
