#include <QTableWidget>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QStandardItemModel>
#include <set>

#include "graphical/ruleview/singlerule/SingleRuleTable.hpp"
#include "paras/Rule.hpp"
#include "graphical/GUI.hpp"
#include "util/Utility.hpp"
#include "graphical/ruleview/singlerule/FilterEditor.hpp"
#include "graphical/ruleview/singlerule/FilterCompleter.hpp"
#include "graphical/ruleview/singlerule/RuleDetailView.hpp"

using namespace PARASProgram;
using namespace PARASProgram::Graphical;

/*
 *  Constructor & Destructor
 */
SingleRuleTable::SingleRuleTable(QWidget *parent) : QFrame(parent)
{
    buildTable();
    buildInfoBox();
}

SingleRuleTable::~SingleRuleTable()
{
    delete tableWidget;
    delete infoBox;
}

/**
 * @brief SingleRuleTable::buildTable creates the rule table portion with the desired setup.
 */
void SingleRuleTable::buildTable()
{
    rowRule = new vector<Rule*>();
    tableModel = new QStandardItemModel(this);
    resetTable();

    tableWidget = new QTableView(this);
    tableWidget->setModel(tableModel);
    tableWidget->setShowGrid(true);
    tableWidget->horizontalHeader()->setStretchLastSection(true);
    tableWidget->setSortingEnabled(true);
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    tableWidget->verticalHeader()->setVisible(false);
    //setup rule selection listening
    QObject::connect(tableWidget->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(updateRuleSelection()));
}



/**
 * @brief SingleRuleTable::buildSearchBox creates the search box below the rule table.
 */
void SingleRuleTable::buildInfoBox()
{
    infoBox = new QFrame(this);
    infoBox->setFrameShape(QFrame::Box);
    infoBox->setFrameShadow(QFrame::Sunken);
    detailView = new RuleDetailView(infoBox);
}

/**
 * @brief RuleTable::populateTable populates the table with the given rules
 * @param rules to be added to the table
 */
void SingleRuleTable::populateTable(set<Rule*> *rules)
{
    resetTable();

    string *buf = new string();
    rowRule->clear();
    int rowCount = 0;
    for(set<Rule*>::iterator i = rules->begin(); i != rules->end(); ++i)
    {
        Rule *ru = *i;

        vector<string*> *toDisplayX = ru->getX();
        if(ru->displayAnte->size() > 0) toDisplayX = ru->displayAnte;
        vector<string*> *toDisplayY = ru->getY();
        if(ru->displayCons->size() > 0) toDisplayY = ru->displayCons;

        QStandardItem *ante = new QStandardItem();
        ante->setFlags(ante->flags() ^ Qt::ItemIsEditable);
        ante->setText(QString::fromStdString(*Utility::arrayToString(toDisplayX, buf)));
        QStandardItem *cons = new QStandardItem();
        cons->setFlags(cons->flags() ^ Qt::ItemIsEditable);
        cons->setText(QString::fromStdString(*Utility::arrayToString(toDisplayY, buf)));
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
        tableModel->insertRow(rowCount, row);
        rowRule->push_back(ru);
        rowCount++;
    }

    detailView->setRuleCount(rules->size());
    normalizeColumnWidths();
    delete buf;
}

/**
 * @brief SingleRuleTable::updateRuleSelection updates the info box to show the information of the currently selected rule.
 */
void SingleRuleTable::updateRuleSelection()
{
    QModelIndexList selectedIndices = tableWidget->selectionModel()->selectedIndexes();
    if(selectedIndices.size() == 0)
    {
        detailView->setSelectedRule(NULL);
        return;
    }
    Rule *ruleForRow = rowRule->at(selectedIndices.at(0).row());
    detailView->setSelectedRule(ruleForRow);
}

/**
 * @brief SingleRuleTable::resetAll clears the contents of the table.
 */
void SingleRuleTable::resetAll()
{
    resetTable();
    normalizeColumnWidths();
    detailView->setRuleCount(-1);
}


/**
 * @brief SingleRuleTable::resetTable resets the table model to the clear state
 */
void SingleRuleTable::resetTable()
{
    tableModel->clear();
    tableModel->setRowCount(0);
    tableModel->setColumnCount(4);
    tableModel->setHorizontalHeaderLabels(QStringList() << "Antecedent" << "Consequent" << "Support" << "Confidence");
}

/**
 * @brief SingleRuleTable::normalizeColumnWidths resizes the column widths based on if there is a selected stable region.
 */
void SingleRuleTable::normalizeColumnWidths()
{
    tableWidget->setColumnWidth(0, tableWidget->width() / 4);
    tableWidget->setColumnWidth(1, tableWidget->width() / 4);
    tableWidget->setColumnWidth(2, tableWidget->width() / 4);
    tableWidget->setColumnWidth(3, tableWidget->width() / 4 - 5);
}

int SingleRuleTable::getColumnWidth(int col)
{
    return tableWidget->columnWidth(col);
}

int SingleRuleTable::getColumnPosition(int col)
{
    return tableWidget->columnViewportPosition(col);
}

/*
 * Event listener functions
 */

void SingleRuleTable::applicationSizeChanged(int width, int height)
{
    this->move(0,0);
    this->resize(width,height);

    //table
    tableWidget->resize(width, height * 2 / 3 - 30);
    tableWidget->move(0, 80);

    //info
    infoBox->resize(width, 130);
    infoBox->move(0, tableWidget->y() + tableWidget->height() + 12);

    detailView->resize(width, 100);

    normalizeColumnWidths();
}

void SingleRuleTable::updateRules(set<Rule *> *newRules)
{
    resetAll();
    detailView->setSelectedRule(NULL);
    if(newRules != NULL)
    {
        populateTable(newRules);
    }
}

void SingleRuleTable::updateAttributes(vector<Attribute*> *attr)
{
    this->detailView->updateAttributes(attr);
}
