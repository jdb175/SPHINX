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

#include "src/Hypo/dempstershafer.hpp"
#include "src/Hypo/learningclassificator.hpp"
#include "src/Hypo/csvreader.hpp"
#include "src/Hypo/sleep.cpp"

using namespace std;
using namespace SPHINXProgram;

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

    //initialize
    //get data file
    Json::Value csvFile = root["datafile"];
    CSVReader csv(csvFile.asString());

    // create Dempster-Shafer universe
    DempsterShaferUniverse universe;

    // create the feature classificator with online learning
    cout << "Processing evidence types\n";
    Json::Value evidenceType = root["evidence_types"];
    int classifications[csv.number_of_columns()];
    //initialize classifications
    for(int i = 0; i < csv.number_of_columns(); ++i){
        classifications[i] = -1;
    }

    set<void*> hypotheses_names;
    bitset<MAX_HYPOTHESESES> *bitsetsMin = new bitset<MAX_HYPOTHESESES>[csv.number_of_columns()];
    bitset<MAX_HYPOTHESESES> *bitsetsMax = new bitset<MAX_HYPOTHESESES>[csv.number_of_columns()];


    LearningClassificator classificator(0.05, evidenceType.size());

    for(Json::Value::iterator evidenceIterator = evidenceType.begin(); evidenceIterator != evidenceType.end(); ++evidenceIterator) {
        Json::Value type = *evidenceIterator;
        double avg = classificator.add_feature(type.get("avg", 0).asDouble());
        classifications[type.get("column", 0).asInt()] = avg;
        cout << "****Adding feature: " << type.get("name", 0).asCString() <<endl;
        cout<<"   *Average = "<< type.get("avg", 0).asDouble() << endl;
        cout<<"   *Column = "<< type.get("column", 0).asInt() << endl;


        //minimum hypos
        Json::Value minVals = type.get("hypo_less", 0);
        set<void*> hypothesesMin;
        //get them from json
        cout << "   *Minimum Hypotheses: ";

        for(Json::Value::iterator minIt = minVals.begin(); minIt != minVals.end(); ++minIt) {
            Json::Value str = *minIt;
            hypothesesMin.insert((void*) new string(str.asString()));
            hypotheses_names.insert((void*) new string(str.asString()));

            if(minIt != minVals.begin()){
                cout << " and";
            }
            cout << " " << str.asString();
        }
        cout << endl;

        //create the bitset
        bitsetsMin[type.get("column", 0).asInt()] = universe.bitset_representation(hypothesesMin);

        //maximum hypos
        Json::Value maxVals = type.get("hypo_more", 0);
        set<void*> hypothesesMax;
        //get them from json
        cout << "   *Maximum Hypotheses: ";
        for(Json::Value::iterator maxIt = maxVals.begin(); maxIt != maxVals.end(); ++maxIt) {
            Json::Value str = *maxIt;
            hypothesesMax.insert((void*) new string(str.asString()));
            hypotheses_names.insert((void*) new string(str.asString()));

            if(maxIt != maxVals.begin()){
                cout << " and";
            }
            cout << " " << str.asString();
        }
        cout << endl;
        //create the bitset
        bitsetsMax[type.get("column", 0).asInt()] = universe.bitset_representation(hypothesesMax);

        cout << "   *" << bitsetsMin[type.get("column", 0).asInt()] << endl;
        cout << "   *" << bitsetsMax[type.get("column", 0).asInt()] << endl;
    }

    //add all hypothesis permutations
    universe.add_hypotheseses(hypotheses_names);

    return;
    //now iterate through csv file
    for(int i=0; i<csv.number_of_rows(); i++) {
        vector<int> frame = csv.row(i);
        vector<Evidence> evidenceItems;

        cout << "Classifying row " << i << endl;

        //Classify features
        for(int j = 0; j < csv.number_of_columns(); ++j){
            //Ignore unused columns
            if(classifications[j] == -1) {
                cout << "    Skipping column " << j << endl;
                continue;
            }

            // evidence
            cout << "    Classifying column " << j << " value of " << frame.at(j) <<", ";
            Evidence curEvidence = universe.add_evidence();
            double evidenceClassification = classificator.classify(classifications[j], frame.at(j));
            evidenceClassification *= 0.9; // we don't want 1.0 as mass
            if(evidenceClassification >= 0.0) {
                // large classification
                cout << "large";
                curEvidence.add_focal_set(evidenceClassification, bitsetsMax[j]);
            } else {
                // small classification
                cout << "small";
                curEvidence.add_focal_set(-evidenceClassification, bitsetsMin[j]);
            }
            curEvidence.add_omega_set();
            evidenceItems.push_back(curEvidence);
            curEvidence.best_match();
            cout << ", " << *((string*) curEvidence.best_match()) << endl;
        }

        //Combine Features
        Evidence combined_features = evidenceItems[0];
        for(vector<Evidence>::size_type j = 1; j < evidenceItems.size(); ++j){
            combined_features = combined_features & evidenceItems[j];
        }

        //classify
        string* hypothesis = (string*) combined_features.best_match();
        cout << "    *** Final Classification: "<< *hypothesis << endl;
    }
}

