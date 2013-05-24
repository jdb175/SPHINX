#include <string>
#include <vector>
#include <sstream>
#include "paras/Rule.hpp"
#include "util/Utility.hpp"
#include "util/Convert.hpp"
#include "util/json/json.h"

using namespace std;
using namespace SPHINXProgram;

Rule::Rule(vector<string*> *strCombination, vector<string*> *strRemaining, double Support, double Confidence, vector<XYPair*> *DominatePoints)
{
    this->displayAnte = NULL;
    this->displayCons = NULL;
	this->strCombination = strCombination;
	this->strRemaining = strRemaining;
	this->_support = Support;
	this->_confidence = Confidence;
	this->_dominatePoint_simple = NULL;
	this->_dominatePoint_strict = NULL;
}

Rule::~Rule()
{
    for(size_t i = 0; i < strCombination->size(); i++) delete strCombination->at(i);
    for(size_t i =0; i < strRemaining->size(); i++) delete strRemaining->at(i);
    delete strCombination;
    delete strRemaining;
    if(_dominatePoint_simple != NULL) delete _dominatePoint_simple;
    if(_dominatePoint_strict != NULL) delete _dominatePoint_strict;
}

vector<string*> *Rule::getX()
{
	return strCombination;
}

vector<string*> *Rule::getY()
{
	return strRemaining;
}

double Rule::getSupport()
{
	return _support;
}

double Rule::getConfidence()
{
	return _confidence;
}

SPHINXProgram::XYPair *Rule::getDominantPointSimple()
{
	return _dominatePoint_simple;
}

void Rule::setDominantPointSimple(XYPair *value)
{
	_dominatePoint_simple = value;
}

SPHINXProgram::XYPair *Rule::getDominantPointStrict()
{
	return _dominatePoint_strict;
}

void Rule::setDominantPointStrict(XYPair *value)
{
	_dominatePoint_strict = value;
}

bool Rule::operator<(Rule *other)
{
	if (_support == other->_support)
		return _confidence < other->_confidence;
	else
		return _support < other->_support;
}

bool Rule::operator>(Rule *other)
{
	if (_support == other->_support)
		return _confidence > other->_confidence;
	else
		return _support > other->_support;
}

bool Rule::operator==(Rule *other)
{
	return _support == other->_support && _confidence == other->_confidence;
}

string* Rule::ToListString()
{
	string *strComb = new string();
	strComb = Utility::arrayToString(strCombination, strComb);
	string strRem = *Utility::arrayToString(strRemaining, &strRem);
    strComb->append(strRem);
	return strComb;
}

string* Rule::XToListString()
{
	string *xListStr = new string();
	xListStr = Utility::arrayToString(strCombination, xListStr);
	return xListStr;
}

string* Rule::YToListString()
{
	string *yListStr = new string();
	yListStr = Utility::arrayToString(strRemaining, yListStr);
	return yListStr;
}

Json::Value Rule::jsonify()
{
    Json::Value root;

	for(size_t i = 0; i < strCombination->size(); i++)
	{
        root["ant"].append(*(strCombination->at(i)));
	}

	for(size_t i = 0; i < strRemaining->size(); i++)
	{
        root["con"].append(*(strRemaining->at(i)));
	}

    root["sup"] = _support;
    root["conf"] = _confidence;

	if(_dominatePoint_simple != NULL)
	{
        root["sim"]["x"] = _dominatePoint_simple->getX();
        root["sim"]["y"] = _dominatePoint_simple->getY();
	}

	if(_dominatePoint_strict != NULL)
	{
        root["dom"]["x"] = _dominatePoint_strict->getX();
        root["dom"]["y"] = _dominatePoint_strict->getY();
	}

    return root;
}

