#include <QCompleter>
#include <QStringListModel>
#include <QSortFilterProxyModel>
#include <iostream>
#include "graphical/ruleview/singlerule/FilterCompleter.hpp"

using namespace PARASProgram::Graphical;

FilterCompleter::FilterCompleter() : QCompleter()
{
    this->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
    this->setCaseSensitivity(Qt::CaseInsensitive);
    this->filterProxyModel = new QSortFilterProxyModel();
    this->stringModel = new QStringListModel(this);
    QCompleter::setModel(filterProxyModel);
}

void FilterCompleter::updateModel(set<vector<string> > items)
{
    if(items.size() == 0) return;
    QStringList itemList;
    for(set<vector<string> >::iterator i = items.begin(); i != items.end(); ++i)
    {
        vector<string> strVec = *i;
        QString itemString;
        for(vector<string>::iterator j = strVec.begin(); j != strVec.end(); ++j)
        {
            itemString.append(QString::fromStdString((*j)));
            if(j + 1 != strVec.end()) itemString.append(',');
        }
        itemList.push_back(itemString);
    }

    stringModel->setStringList(itemList);
    filterProxyModel->setSourceModel(stringModel);
}

QStringList FilterCompleter::splitPath(const QString &s) const
{
    filterProxyModel->setFilterFixedString(s);

    return QStringList();
}
