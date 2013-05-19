#include <map>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

#include "paras/PSpaceIndex.hpp"
#include "util/Convert.hpp"
#include "paras/Rule.hpp"
#include "util/Utility.hpp"
#include "util/json/json.h"

using namespace std;
using namespace PARASProgram;

/**
 * Constructor if there is not an already existing file for the pspaceindex.
 */
PSpaceIndex::PSpaceIndex(string* forFile, bool pregen)
{
	initializeInstanceFields(forFile);

    if(pregen) loadPSpaceIndexFromFile(forFile);
}

/**
 * @brief PSpaceIndex::~PSpaceIndex destructor
 */
PSpaceIndex::~PSpaceIndex()
{
    for(map<string, set<Rule*>* >::const_iterator it = preStoredRules->begin(); it != preStoredRules->end(); ++it)
    {
        set<Rule*> *temp = it->second;

        for(set<Rule*>::iterator i = temp->begin(); i != temp->end(); ++i) delete (*i);

        delete temp;
    }

    for(size_t i = 0; i < paramSpacePoints->size(); i++) delete paramSpacePoints->at(i);
    paramSpacePoints->clear();

    for(size_t i = 0; i < attributes->size(); i++) delete attributes->at(i);

    delete preStoredRules;
    delete stableRegionPoints;
    delete paramSpacePoints;
    delete fileName;

    delete attributes;

    preStoredRules = NULL;
    stableRegionPoints = NULL;
    paramSpacePoints = NULL;
    fileName = NULL;
}

/**
 * Initializes all of the fields/parameters used within the PSpaceIndex.
 */
void PSpaceIndex::initializeInstanceFields(string *file)
{
	paramSpacePoints = new vector<XYPair*>();
	stableRegionPoints = new vector<XYPair*>();
    preStoredRules = new map<string, set<Rule*>*>();
    attributes = new vector<Attribute*>();
	fileName = file;
	lastTransactionID = 3196; //For PARAS m_nLastTransID=#transaction, for Apriori m_nLastTransID=1 //TODO
}

/**
 * @brief PSpaceIndex::addAttribute adds the specified attribute to the index.
 * @param toAdd the attribute to be added.
 */
void PSpaceIndex::addAttribute(Attribute *toAdd)
{
    attributes->push_back(toAdd);
}

/**
 * Returns all attributes in the pspaceindex.
 */
vector<Attribute*> *PSpaceIndex::getAttributeList()
{
    return attributes;
}

/**
 * Add the specified list of points to the stable region point list.
 */
void PSpaceIndex::setStableRegionPoints(vector<XYPair*> *pointList)
{
    delete stableRegionPoints;
    stableRegionPoints = pointList;
}

/**
 *Returns the stable region points associated with this pspaceindex.
 */
vector<XYPair*> *PSpaceIndex::getStableRegionPoints()
{
    return stableRegionPoints;
}

/**
 * Add the specified list of points to the param space points list.
 */
void PSpaceIndex::setPSpacePoints(vector<XYPair*> *pointList)
{
    delete paramSpacePoints;
    paramSpacePoints = pointList;
}

/**
 * Set the prestored rules to the specified rule list.
 */
void PSpaceIndex::preStoreRules(map<string, set<Rule*>*> *ruleList)
{
    delete preStoredRules;
    preStoredRules = ruleList;
}

/**
 * Return the file name correspondin to this pspace index.
 */
string* PSpaceIndex::getFileName()
{
    return fileName;
}

/**
 * Return the file name correspondin to this pspace index.
 */
void PSpaceIndex::setFileName(string *fName)
{
    fileName = fName;
}

set<Rule*> *PSpaceIndex::getRulesForStableRegion(XYPair* sr)
{
    string *threshold = getHashForSupportConfidence(sr->getX(), sr->getY());
    set<Rule*> *results = (*preStoredRules)[*threshold];

    delete threshold;

    return results;
}


/**
 * Finds the rules that have the given min confidence and min support.
 *  Either returns with redundancy or without depending on third parameter.
 */
set<Rule*> *PSpaceIndex::findRulesForSupportConfidence(double minSupport, double minConfidence, bool withRedundancy)
{
	if(withRedundancy)
	{
		return findRules_wr(minSupport, minConfidence);
	}
	else
	{
		return findRules(minSupport, minConfidence);
	}
}

/**
 * Finds rules that fall in the specified minSupport and min confidence thresholds.
 * @param support the minimum support threshold to be allowed.
 * @param confidence the minimum confidence threshold to be allowed.
 * @return a list of rules that fall above these thresholds.
 */
set<Rule*> *PSpaceIndex::findRules(double minSupport, double minConfidence)
{
    set<Rule*> *rules = new set<Rule*>();
    set<Rule*> *ruleCollection;

	int k = 0;
	for(unsigned int i = 0; i < stableRegionPoints->size(); i++)
	{

		XYPair *item = stableRegionPoints->at(i);
		//check if the item has the required support and confidence.
		if ((item->getX() >= minSupport) && (item->getY() >= minConfidence))
		{
			//get the rules that correspond with the found threshold index.
			string *index = getHashForSupportConfidence(item->getX(), item->getY());
            ruleCollection = (*preStoredRules)[*index];
			delete index; //free the index, it wont be used anymore.

			k+= ruleCollection->size();

            for(set<Rule*>::iterator j = ruleCollection->begin(); j != ruleCollection->end(); ++j)
			{
                Rule *rule = *j;

				//if the rule has a dominating point, it can only be added as a rule if its dominating point's support or confidence
				//do not fall above the min support and confidence. Otherwise, its dominant point should be added instead later on.
				if ((rule->getDominantPointSimple() == NULL) && (rule->getDominantPointStrict() == NULL))
				{
                    rules->insert(rule);
				}
				else if((rule->getDominantPointSimple() == NULL) && (rule->getDominantPointStrict() != NULL))
				{
                    if (rule->getDominantPointStrict()->getX() < minSupport || rule->getDominantPointStrict()->getY() < minConfidence) rules->insert(rule);
				}
				else if ((rule->getDominantPointSimple() != NULL) && (rule->getDominantPointStrict() == NULL))
				{
                    if ((rule->getDominantPointSimple()->getX() < minSupport) || (rule->getDominantPointSimple()->getY() < minConfidence)) rules->insert(rule);
				}
				else
				{
					if (((rule->getDominantPointSimple()->getX() < minSupport) || (rule->getDominantPointSimple()->getY() < minConfidence))
						&& ((rule->getDominantPointStrict()->getX() < minSupport) || (rule->getDominantPointStrict()->getY() < minConfidence)))
                        rules->insert(rule);
				}
			}
		}
	}

	return rules;
}

/**
 * Finds rules that fall in the specified minSupport and min confidence thresholds, with redundancies.
 * @param minSupport the minimum support to be allowed
 * @param minConfidence the minimum confidence to be allowed.
 * @return a list of rules that fall above these minimums, including any redundancies.
 */
set<Rule*> *PSpaceIndex::findRules_wr(double minSupport, double minConfidence)
{
    set<Rule*> *rules = new set<Rule*>();
    set<Rule*> *ruleCollection;

	int k = 0;
	for(unsigned int i = 0; i < stableRegionPoints->size(); i++)
	{
        XYPair *item = stableRegionPoints->at(i);
		//if the support and confidence are greater than the mins allowed, add all of the rules associated
		//with that point
		if ((item->getX() >= minSupport) && (item->getY() >= minConfidence))
		{
			string* index = getHashForSupportConfidence(item->getX(), item->getY());
            ruleCollection = (*preStoredRules)[*index];
			delete index; //free the index, it wont be used anymore.
            if(ruleCollection){
                k+= ruleCollection->size();

                for(set<Rule*>::iterator r = ruleCollection->begin(); r != ruleCollection->end(); ++r)
                {
                    Rule *rule = (*r);
                    rules->insert(rule);
                }
            }
		}
	}

	return rules;
}

/**
 * Calculates the hash value for the corresponding support and confidence for indexing into the pspace.
 */
string* PSpaceIndex::getHashForSupportConfidence(double support, double confidence)
{
	double supp;
	int s, c;
	double doublesupp, doubleconf;
	long long threshold_long;
	supp = support / (lastTransactionID) * 100;

	s = (int)(supp * 1000);
	c = (int)(confidence * 1000000);
	doublesupp = s;
	doubleconf = c;
	threshold_long = (long long)(doublesupp * 10000000 + doubleconf);

	return Convert::longToString(threshold_long);
}

/**
 * Outputs the contents of the PSpaceIndex to a file.
 */
void PSpaceIndex::savePSpaceIndexToFile(string file, bool compressed)
{
    this->fileName = new string(file);
	cout << "Saving PSpaceIndex." << endl;

    Json::Value root;
    Json::Writer *writer;

    if(compressed) {
        writer = new Json::FastWriter();
    } else {
        writer = new Json::StyledWriter();
    }

    root["file"] = file;

    //store the rules
    for(map<string, set<Rule*>*>::iterator j = preStoredRules->begin(); j != preStoredRules->end(); ++j)
    {
        Json::Value thisRule;
        string key = j->first;
        set<Rule*> *rules = j->second;

        thisRule["key"] = key;

        for(set<Rule*>::iterator k = rules->begin(); k != rules->end(); ++k)
        {
            Rule* rule = *k;
            thisRule["vals"].append(rule->jsonify());
        }

        root["rules"].append(thisRule);
    }

    //store the points.

    for(size_t i = 0; i < this->stableRegionPoints->size(); i++)
    {
        XYPair *current = this->stableRegionPoints->at(i);
        root["points"].append(current->jsonify());
    }

    //store the attributes.

    for(size_t i = 0; i < attributes->size(); i++)
    {
        Attribute *current = attributes->at(i);
        root["attr"].append(current->jsonify());
    }

    ofstream fileOut;
    fileOut.open(file.c_str());

    if(fileOut.good() && fileOut.is_open())
    {
        fileOut << writer->write(root);
    }
    else
    {
        cout << "Error saving pspace index" << endl;
    }
}

/**
 * Load the PSpaceIndex from the given file.
 */
void PSpaceIndex::loadPSpaceIndexFromFile(string* file)
{
    Json::Value root;
    Json::Reader reader;

    ifstream index(file->c_str());

    //read all of saved file.
    string fileContents;
    if (index)
    {
        index.seekg(0, std::ios::end);
        fileContents.resize(index.tellg());
        index.seekg(0, std::ios::beg);
        index.read(&fileContents[0], fileContents.size());
        index.close();
    }

    bool parsed = reader.parse(fileContents, root);

    if(!parsed)
    {
        cout << "Error loading PSpace Index File." << endl;
        return;
    }

    //load pspacepoints
    Json::Value xyPair = root["points"];
    for(Json::Value::iterator xyIterator = xyPair.begin(); xyIterator != xyPair.end(); ++xyIterator)
    {
        Json::Value xyVal = *xyIterator;
        double x = xyVal.get("x", 0).asDouble();
        double y = xyVal.get("y",0).asDouble();
        XYPair *newPair = new XYPair(x,y);
        paramSpacePoints->push_back(newPair);
    }

    //set the stable region points
    this->setStableRegionPoints(Utility::distinctPoints(paramSpacePoints));

    //load the rules.
    Json::Value ruleMap = root["rules"];
    for(Json::Value::iterator ruleMapIterator = ruleMap.begin(); ruleMapIterator != ruleMap.end(); ++ruleMapIterator)
    {
        Json::Value currentMap = *ruleMapIterator;
        string key = currentMap.get("key", "null").asString();
        set<Rule*> *rulesSet = new set<Rule*>();
        Json::Value rules = currentMap["vals"];
        for(Json::Value::iterator ruleIterator = rules.begin(); ruleIterator != rules.end(); ++ruleIterator)
        {
            Json::Value currentRule = *ruleIterator;
            vector<string*>* stringAnte = new vector<string*>();
            vector<string*>* stringCons = new vector<string*>();

            Json::Value ante = currentRule["ant"];
            for(Json::Value::iterator antIterator = ante.begin(); antIterator != ante.end(); ++antIterator)
            {
                Json::Value currentAnte = *antIterator;
                string *anteMember = new string( currentAnte.asString());
                stringAnte->push_back(anteMember);
            }

            Json::Value con = currentRule["con"];
            for(Json::Value::iterator conIterator = con.begin(); conIterator != con.end(); ++conIterator)
            {
                Json::Value currentCon = *conIterator;
                string *conMember = new string( currentCon.asString());
                stringCons->push_back(conMember);
            }

            double support = currentRule.get("sup", 0).asDouble();
            double conf = currentRule.get("conf", 0).asDouble();

            Rule *newRule = new Rule(stringAnte, stringCons, support, conf, NULL);

            double simpleX = currentRule["sim"].get("x", -1).asDouble();
            double simpleY = currentRule["sim"].get("y", -1).asDouble();
            double domX = currentRule["dom"].get("x", -1).asDouble();
            double domY = currentRule["dom"].get("y", -1).asDouble();

            if(simpleX != -1 && simpleY != -1)
            {
                XYPair *simple = new XYPair(simpleX, simpleY);
                newRule->setDominantPointSimple(simple);
            }

            if(domX != -1 && domY != -1)
            {
                XYPair *dom = new XYPair(domX, domY);
                newRule->setDominantPointStrict(dom);
            }


            rulesSet->insert(newRule);
        }
        this->preStoredRules->insert(std::pair<string, set<Rule*>*>(key, rulesSet));
    }

    //load attributes
    Json::Value attributes = root["attr"];
    for(Json::Value::iterator att = attributes.begin(); att != attributes.end(); ++att)
    {
        Json::Value currentAttr = *att;
        string attrName = currentAttr.get("name","").asString();
        Json::Value vals = currentAttr["val"];
        vector<string> *values = new vector<string>();
        for(Json::Value::iterator curr = vals.begin(); curr != vals.end(); ++curr)
        {
            Json::Value currentVal = *curr;
            string value = currentVal.asString();
            values->push_back(value);
        }

        Json::Value name = currentAttr["names"];
        vector<string> *names = new vector<string>();
        for(Json::Value::iterator curr = name.begin(); curr != name.end(); ++curr)
        {
            Json::Value currentVal = *curr;
            string value = currentVal.asString();
            names->push_back(value);
        }


        Attribute *newAttr = new Attribute(new string(attrName),values,names);

        this->addAttribute(newAttr);
    }
}

