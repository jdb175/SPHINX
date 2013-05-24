#include "paras/Attribute.hpp"
#include "util/Convert.hpp"
#include "util/json/json.h"

#include <algorithm>
#include <limits>
#include <iostream>
#include <string>

using namespace SPHINXProgram;

Attribute::Attribute(string* name, vector<string> *attributes, vector<string> *names)
{
    this->attributeMembers = attributes;
    this->name = name;
    this->memberNames = names;
    this->minAttr = std::numeric_limits<int>::max();
    this->maxAttr = std::numeric_limits<int>::min();
    for(vector<string>::iterator j = attributes->begin(); j != attributes->end(); ++j)
    {
        int jVal = Convert::stringToInt(&(*j));
        if(jVal < minAttr) minAttr = jVal;
        if(jVal > maxAttr) maxAttr = jVal;
    }
}

void Attribute::convertToNamed(vector<string*> *toBeConverted, vector<string*> *putHere)
{
    for(vector<string*>::iterator i = toBeConverted->begin(); i != toBeConverted->end(); ++i)
    {
        string *valPtr = *i;
        string val = *valPtr;
        string *nameForValue = new string(getNameForValue(val));
        if(nameForValue->compare("") != 0)
        {
            (*nameForValue) = *name+ "="+(*nameForValue).substr(0,3);
            putHere->push_back(nameForValue);
        }
    }
}

string* Attribute::getAttributeName()
{
    return this->name;
}

string Attribute::detailText(vector<string*> *fromVector, int count)
{
    string *fromCount = Convert::intToString(count);
    for(vector<string*>::iterator i = fromVector->begin(); i != fromVector->end(); ++i)
    {
        string val = *(*i);
        string nameForValue = getNameForValue(val);
        if(nameForValue.compare("") != 0)
        {
            return "<b>(" + *fromCount + ") " + *this->name + "</b>" + ": " + nameForValue + "<br>";
        }
    }
    return "";
}

string Attribute::getNameForValue(string member)
{
    vector<string>::iterator locale = std::find(attributeMembers->begin(), attributeMembers->end(), member);
    if(locale == attributeMembers->end()) return "";

    int index = locale - attributeMembers->begin();

    return memberNames->at(index);
}

string Attribute::getValueForName(string member)
{
    vector<string>::iterator locale = std::find(attributeMembers->begin(), attributeMembers->end(), member);
    if(locale == memberNames->end()) return "";

    int index = locale - memberNames->begin();

    return attributeMembers->at(index);
}

bool Attribute::isMember(string possibleMember)
{
    return std::find(attributeMembers->begin(), attributeMembers->end(), possibleMember) != attributeMembers->end();
}

double Attribute::getRelativePosition(string attribute)
{
    string realAttr = attribute;
    if(!isMember(realAttr)) return 1;
    if(maxAttr == minAttr) return 1;
    return (Convert::stringToInt(&realAttr) - minAttr) / (maxAttr - minAttr);
}


Json::Value  Attribute::jsonify()
{
    Json::Value attrRoot;
    attrRoot["name"] = *this->name;
    attrRoot["mapped"] = this->mapped;
    for(size_t i = 0; i < this->attributeMembers->size(); i++)
    {
        attrRoot["val"].append((attributeMembers->at(i)));
    }

    for(size_t i = 0; i < this->memberNames->size(); i++)
    {
        attrRoot["names"].append((memberNames->at(i)));
    }

    return attrRoot;
}

