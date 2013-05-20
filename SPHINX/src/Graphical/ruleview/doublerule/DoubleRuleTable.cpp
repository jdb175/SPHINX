#include <algorithm>
#include <QHeaderView>

#include "graphical/ruleview/doublerule/DoubleRuleTable.hpp"
#include "graphical/EventController.hpp"
#include "util/Utility.hpp"

using namespace PARASProgram::Graphical;

DoubleRuleTable::DoubleRuleTable(QWidget *par, EventController *evCont) : QFrame(par)
{
    interLabel = new QLabel(this);
    interLabel->setText(QString("<b>Intersection of A & B:</b>"));
    interLabel->resize(1000,20);
    interTable = new QTableView(this);
    interModel = new QStandardItemModel();
    interTable->setModel(interModel);
    interModel->setHorizontalHeaderLabels(QStringList() << "Antecedent" << "Consequent" << "Support" << "Confidence");
    setupTable(interTable);

    primaryLabel = new QLabel(this);
    QPalette primaryPalette = primaryLabel->palette();
    primaryPalette.setColor(primaryLabel->foregroundRole(), QColor(0,0,0,255));
    primaryLabel->setPalette(primaryPalette);
    primaryLabel->setText(QString("<b>Unique in Selection A:</b>"));
    primaryLabel->resize(1000,20);
    primaryTable = new QTableView(this);
    primaryModel = new QStandardItemModel();
    primaryTable->setModel(primaryModel);
    primaryModel->setHorizontalHeaderLabels(QStringList() << "Antecedent" << "Consequent" << "Support" << "Confidence");
    setupTable(primaryTable);

    secondaryLabel = new QLabel(this);
    QPalette secondaryPalette = secondaryLabel->palette();
    secondaryPalette.setColor(secondaryLabel->foregroundRole(), QColor(155,155,155,255));
    secondaryLabel->setPalette(secondaryPalette);
    secondaryLabel->setText(QString("<b>Unique in Selection B:</b>"));
    secondaryLabel->resize(1000,20);
    secondaryTable = new QTableView(this);
    secondaryModel = new QStandardItemModel();
    secondaryTable->setModel(secondaryModel);
    secondaryModel->setHorizontalHeaderLabels(QStringList() << "Antecedent" << "Consequent" << "Support" << "Confidence");
    setupTable(secondaryTable);

    mode = RuleMode(UNIQUE);
    redun = false;

    primarySelection = NULL;
    secondarySelection = NULL;
}

void DoubleRuleTable::setupTable(QTableView *table)
{
    table->horizontalHeader()->setStretchLastSection(true);
    table->setShowGrid(true);
    table->setSortingEnabled(true);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->verticalHeader()->setVisible(false);
}

void DoubleRuleTable::populateTable(QStandardItemModel *model, set<Rule*> *rules)
{
    model->removeRows(0, model->rowCount());

    string *buf = new string();
    int rowCount = 0;
    for(set<Rule*>::iterator j = rules->begin(); j != rules->end(); ++j)
    {
        Rule* ru = *j;

        if(ru->displayAnte == NULL || ru->displayCons == NULL)
        {
            vector<string*> *toBeNamedAnte = new vector<string*>();
            vector<string*> *toBeNamedCons = new vector<string*>();

            for(vector<Attribute*>::iterator a = this->attributes->begin(); a != this->attributes->end(); ++a)
            {
                Attribute* curr = *a;
                curr->convertToNamed(ru->getX(), toBeNamedAnte);
                curr->convertToNamed(ru->getY(), toBeNamedCons);
            }

            ru->displayAnte = toBeNamedAnte;
            ru->displayCons = toBeNamedCons;

        }

        vector<string*> *displayX = ru->getX();
        vector<string*> *displayY = ru->getY();
        if(ru->displayAnte->size() > 0) displayX = ru->displayAnte;
        if(ru->displayCons->size() > 0) displayY = ru->displayCons;

        QStandardItem *ante = new QStandardItem();
        ante->setFlags(ante->flags() ^ Qt::ItemIsEditable);
        ante->setText(QString::fromStdString(*Utility::arrayToString(displayX, buf)));
        QStandardItem *cons = new QStandardItem();
        cons->setFlags(cons->flags() ^ Qt::ItemIsEditable);
        cons->setText(QString::fromStdString(*Utility::arrayToString(displayY, buf)));
        QStandardItem *sup = new QStandardItem();
        sup->setFlags(sup->flags() ^ Qt::ItemIsEditable);
        sup->setText(QString::number(ru->getSupport()));
        QStandardItem *conf = new QStandardItem();
        conf->setFlags(conf->flags() ^ Qt::ItemIsEditable);
        conf->setText(QString::number(ru->getConfidence()));

        QList<QStandardItem*> row;
        row.push_back(ante);
        row.push_back(cons);
        row.push_back(sup);
        row.push_back(conf);
        model->insertRow(rowCount, row);
        rowCount++;
    }

    delete buf;
    delete rules;
}

void DoubleRuleTable::resetAll()
{
    interModel->removeRows(0, interModel->rowCount());
    primaryModel->removeRows(0, primaryModel->rowCount());
    secondaryModel->removeRows(0, secondaryModel->rowCount());
}


//event listeners
void DoubleRuleTable::selectStableRegions(Nugget *primarySR, Nugget *secondarySR)
{
    primarySelection = primarySR;
    secondarySelection = secondarySR;
    if(primarySelection == NULL || secondarySelection == NULL) return;

    primaryDiff = new set<Rule*>();
    secondaryDiff = new set<Rule*>();
    intersection = new set<Rule*>();
    set<Rule*> *primRules = primarySR->getRules(mode, redun);
    set<Rule*> *secRules = secondarySR->getRules(mode, redun);

    std::set_difference(primRules->begin(), primRules->end(),
                        secRules->begin(), secRules->end(),
                        std::inserter((*primaryDiff), primaryDiff->end()));

    std::set_difference(secRules->begin(), secRules->end(),
                        primRules->begin(), primRules->end(),
                        std::inserter((*secondaryDiff), secondaryDiff->end()));

    std::set_intersection(primRules->begin(), primRules->end(),
                          secRules->begin(), secRules->end(),
                          std::inserter((*intersection), intersection->end()));

    populateTable(primaryModel, primaryDiff);
    primaryLabel->setText(QString("<b>Unique in Selection A:</b> ").append(QString::number(primaryDiff->size())));
    populateTable(secondaryModel, secondaryDiff);
    secondaryLabel->setText(QString("<b>Unique in Selection B:</b> ").append(QString::number(secondaryDiff->size())));
    populateTable(interModel, intersection);
    interLabel->setText(QString("<b>Intersection of A & B:</b> ").append(QString::number(intersection->size())));
    repaint();
}

void DoubleRuleTable::applicationSizeChanged(int width, int height)
{
    this->move(0,50);
    this->resize(width,height);

    primaryLabel->move(0,0);
    primaryTable->move(7,primaryLabel->y() + primaryLabel->height() + 5);
    primaryTable->resize(width - 10, height / 4);

    secondaryLabel->move(0,primaryTable->y() + primaryTable->height() + 5);
    secondaryTable->move(7, secondaryLabel->y() + secondaryLabel->height() + 5);
    secondaryTable->resize(width - 10, height / 4);

    interLabel->move(0, secondaryTable->y() + secondaryTable->height() + 5);
    interTable->move(7, interLabel->y() + interLabel->height() + 5);
    interTable->resize(width - 10, height / 4 );

    normalizeColumnWidths(primaryTable);
    normalizeColumnWidths(secondaryTable);
    normalizeColumnWidths(interTable);
}

/**
 * @brief DoubleRuleTable::normalizeColumnWidths resizes the column widths based on if there is a selected stable region.
 */
void DoubleRuleTable::normalizeColumnWidths(QTableView *tableWidget)
{
    tableWidget->setColumnWidth(0, tableWidget->width() / 4);
    tableWidget->setColumnWidth(1, tableWidget->width() / 4);
    tableWidget->setColumnWidth(2, tableWidget->width() / 4);
    tableWidget->setColumnWidth(3, tableWidget->width() / 4 - 5);
}

void DoubleRuleTable::setRuleMode(RuleMode ruleMode)
{
    this->mode = ruleMode;
    this->selectStableRegions(primarySelection, secondarySelection);
}

void DoubleRuleTable::updateIndexInfo(IndexUpdateEvent e)
{
    this->attributes = e.attributes;
    resetAll();
}

void DoubleRuleTable::closeIndex()
{
    resetAll();
}

void DoubleRuleTable::updateRedundancy(bool include)
{
    this->redun = include;
    this->selectStableRegions(primarySelection, secondarySelection);
}

