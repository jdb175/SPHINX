#include <string>
#include <vector>
#include <sstream>
#include "paras/Nugget.hpp"
#include "util/Utility.hpp"
#include "util/Convert.hpp"
#include "util/json/json.h"

using namespace std;
using namespace SPHINXProgram;

Nugget::Nugget(double belief, double plausiblity, string hypothesis, string evidenceType, vector<double> values)
{
    this->_belief = belief;
    this->_plausibility = plausibility;
    this->hypothesis = hypothesis;
    this->evidenceType = evidenceType;
    this->values = values;
}

Nugget::~Nugget()
{
}

double Nugget::getBelief()
{
    return _belief;
}

double Nugget::getPlausibility()
{
    return _plausiblity;
}

bool Nugget::operator<(Nugget *other)
{
    if (_belief == other->_belief)
        return _plausibility < other->_plausibility;
	else
        return _belief < other->_belief;
}

bool Nugget::operator>(Nugget *other)
{
    if (_belief == other->_belief)
        return _plausibility > other->_plausibility;
	else
        return _belief > other->_belief;
}

bool Nugget::operator==(Nugget *other)
{
    return _belief == other->_belief && _plausibility == other->_plausibility;
}


Json::Value Nugget::jsonify()
{
    Json::Value root;

    /*for(size_t i = 0; i < strCombination->size(); i++)
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
    }*/

    return root;
}

