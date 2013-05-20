#ifndef FILTEREDITOR_HPP
#define FILTEREDITOR_HPP

#include <QLineEdit>
#include <QStandardItemModel>
#include <vector>
#include <string>
#include "FilterCompleter.hpp"

using namespace std;

namespace PARASProgram
{
namespace Graphical
{
    class FilterEditor : public QLineEdit
    {
        Q_OBJECT
        public:
            explicit FilterEditor(QWidget *parent = 0);
            void setupFilter();
            vector<string> *getFilter();
            void resetFilter();
            bool isValid(vector<string*> *toCheck);

        private:
            vector<string> *filter;
        protected:

    };
}
}


#endif // FILTEREDITOR_HPP
