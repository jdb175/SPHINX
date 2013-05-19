#ifndef FILTERCOMPLETER_HPP
#define FILTERCOMPLETER_HPP

#include <set>
#include <vector>
#include <QCompleter>
#include <QStringListModel>
#include <QSortFilterProxyModel>

using namespace std;
namespace PARASProgram
{
namespace Graphical
{
    class FilterCompleter : public QCompleter
    {
        Q_OBJECT
        public:
            explicit FilterCompleter();
            void updateModel(set<vector<string> > items);
            void setModel(QStringListModel *model, QStringList *list);
            QStringList splitPath(const QString &s) const;

        private:
            QSortFilterProxyModel *filterProxyModel;
            QStringListModel *stringModel;
    };
}
}

#endif // FILTERCOMPLETER_HPP
