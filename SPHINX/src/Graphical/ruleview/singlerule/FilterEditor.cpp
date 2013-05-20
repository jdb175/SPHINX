#include <sstream>

#include "graphical/ruleview/singlerule/FilterEditor.hpp"
#include "graphical/ruleview/singlerule/FilterCompleter.hpp"
#include "util/Utility.hpp"

using namespace PARASProgram::Graphical;

FilterEditor::FilterEditor(QWidget *parent) : QLineEdit(parent)
{
    filter = new vector<string>();
}

void FilterEditor::setupFilter()
{
    filter->clear();
    //split the string at spaces
    stringstream splitstring(text().toStdString());
    string tmpstr;
    while(getline(splitstring, tmpstr, ','))
    {
        filter->push_back(*Utility::bothTrim(&tmpstr));
    }
}

void FilterEditor::resetFilter()
{
    filter->clear();
}

vector<string> *FilterEditor::getFilter()
{
    return filter;
}

bool FilterEditor::isValid(vector<string*> *toCheck)
{
    if(filter->size() == 0) return true;
    if(filter->size() > toCheck->size()) return false;

    for(vector<string>::iterator j = filter->begin(); j != filter->end(); ++j)
    {
        string toFind = *j;
        bool haveFound = false;
        for(vector<string*>::iterator k = toCheck->begin(); k != toCheck->end(); ++k)
        {
            string checking = *Utility::bothTrim(*k);
            if(toFind.compare(checking) == 0)
            {
                haveFound = true;
                break;
            }
        }

        if(!haveFound)
        {
            return false;
        }
    }

    return true;
}


