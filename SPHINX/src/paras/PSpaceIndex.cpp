#include <map>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

#include "paras/PSpaceIndex.hpp"
#include "util/Convert.hpp"
#include "paras/Nugget.hpp"
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

    if(pregen) {
        loadPSpaceIndexFromFile(forFile);
    } else {
        generatePSpaceIndexFromFile(forFile);
    }
}

/**
 * @brief PSpaceIndex::~PSpaceIndex destructor
 */
PSpaceIndex::~PSpaceIndex()
{
    for(map<string, set<Nugget*>* >::const_iterator it = hypothesisNuggets->begin(); it != hypothesisNuggets->end(); ++it)
    {
        set<Nugget*> *temp = it->second;

        for(set<Nugget*>::iterator i = temp->begin(); i != temp->end(); ++i) delete (*i);

        delete temp;
    }

    for(set<Nugget*>::iterator i = allNuggets->begin(); i != allNuggets->end(); ++i) delete (*i);


    delete fileName;
    fileName = NULL;
}

/**
 * Initializes all of the fields/parameters used within the PSpaceIndex.
 */
void PSpaceIndex::initializeInstanceFields(string *file)
{
    allNuggets = new set<Nugget*>();
    hypothesisNuggets = new map<string, set<Nugget*>*>();
	fileName = file;
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



/**
 * Outputs the contents of the PSpaceIndex to a file.
 */
void PSpaceIndex::savePSpaceIndexToFile(string file, bool compressed)
{
    /*this->fileName = new string(file);
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
    }*/
}

/**
 * @brief PSpaceIndex::loadPSpaceIndexFromFile loads an exising
 * pSpaceIndex from the given file
 * @param file
 */
void PSpaceIndex::loadPSpaceIndexFromFile(string* file)
{
    //TODO make this a thing
    generatePSpaceIndexFromFile(file);
}


/**
 * @brief PSpaceIndex::generatePSpaceIndexFromFile generates a pSpaceIndex
 * from the given file
 * @param file the file to load from
 */
void PSpaceIndex::generatePSpaceIndexFromFile(string* file)
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
    cout << "***Processing evidence types***\n";
    Json::Value evidenceType = root["evidence_types"];
    int classifications[csv.number_of_columns()];
    //initialize classifications
    for(int i = 0; i < csv.number_of_columns(); ++i){
        classifications[i] = -1;
    }

    set<void*> hypotheses_names;
    set<void*> *maxSets = new set<void*>[csv.number_of_columns()];
    set<void*> *minSets = new set<void*>[csv.number_of_columns()];

    bitset<MAX_HYPOTHESESES> *bitsetsMin = new bitset<MAX_HYPOTHESESES>[csv.number_of_columns()];
    bitset<MAX_HYPOTHESESES> *bitsetsMax = new bitset<MAX_HYPOTHESESES>[csv.number_of_columns()];


    LearningClassificator classificator(0.05, evidenceType.size());

    for(Json::Value::iterator evidenceIterator = evidenceType.begin(); evidenceIterator != evidenceType.end(); ++evidenceIterator) {
        Json::Value type = *evidenceIterator;
        double avg = classificator.add_feature(type.get("avg", 0).asDouble());
        classifications[type.get("column", 0).asInt()] = avg;
        cout << "Adding feature '" << type.get("name", 0).asCString() << "':"<< endl;
        cout <<"    Average = "<< type.get("avg", 0).asDouble() << "." << endl;
        cout <<"    Column = "<< type.get("column", 0).asInt() << "." << endl;


        //minimum hypos
        Json::Value minVals = type.get("hypo_less", 0);
        cout << "    Minimum Hypotheses:"<<endl;
        //get them from json
        for(Json::Value::iterator minIt = minVals.begin(); minIt != minVals.end(); ++minIt) {
            Json::Value str = *minIt;
            void* val = addHypothesisObject(str.asString(), &hypotheses_names);
            minSets[type.get("column", 0).asInt()].insert(val);
        }

        //maximum hypos
        Json::Value maxVals = type.get("hypo_more", 0);
        cout << "    Maximum Hypotheses:"<<endl;
        //get them from json
        for(Json::Value::iterator maxIt = maxVals.begin(); maxIt != maxVals.end(); ++maxIt) {
            Json::Value str = *maxIt;
            void* val = addHypothesisObject(str.asString(), &hypotheses_names);
            maxSets[type.get("column", 0).asInt()].insert(val);
        }
        cout << endl;
    }

    //add all hypotheses
    universe.add_hypotheseses(hypotheses_names);

    //create bitsets
    cout << "Bitsets:\n";
    for(Json::Value::iterator evidenceIterator = evidenceType.begin(); evidenceIterator != evidenceType.end(); ++evidenceIterator) {
        Json::Value type = *evidenceIterator;
        bitsetsMax[type.get("column", 0).asInt()] = universe.bitset_representation(maxSets[type.get("column", 0).asInt()]);
        bitsetsMin[type.get("column", 0).asInt()] = universe.bitset_representation(minSets[type.get("column", 0).asInt()]);

        cout << "    " << bitsetsMax[type.get("column", 0).asInt()] << endl;
        cout << "    " << bitsetsMin[type.get("column", 0).asInt()] << endl;
    }
    cout << endl;

    cout << "***Processing input file***\n";
    cout << "File path: '" << csvFile.asString() <<"'.\n";
    //now iterate through csv file
    for(int i=0; i<csv.number_of_rows(); i++) {
        vector<int> frame = csv.row(i);
        vector<Evidence> evidenceItems;

        cout << "Classifying row " << i << ":" << endl;

        //Classify features
        for(int j = 0; j < csv.number_of_columns(); ++j){
            //Ignore unused columns
            if(classifications[j] == -1) {
                cout << "    Skipping column " << j << "." << endl;
                continue;
            }

            // evidence
            cout << "    Classifying col. " << j << " value of " << frame.at(j) <<", ";
            Evidence curEvidence = universe.add_evidence();
            double evidenceClassification = classificator.classify(classifications[j], frame.at(j));
            evidenceClassification *= 0.9; // we don't want 1.0 as mass
            if(evidenceClassification >= 0.0) {
                // large classification
                cout << "> avg.\n";
                curEvidence.add_focal_set(evidenceClassification, bitsetsMax[j]);
            } else {
                // small classification
                cout << "< avg.\n";
                curEvidence.add_focal_set(-evidenceClassification, bitsetsMin[j]);
            }
            curEvidence.add_omega_set();
            evidenceItems.push_back(curEvidence);
            curEvidence.best_match();
        }

        //Combine Features
        Evidence combined_features = evidenceItems[0];
        for(vector<Evidence>::size_type j = 1; j < evidenceItems.size(); ++j){
            combined_features = combined_features & evidenceItems[j];
        }

        //classify
        string* hypothesis = (string*) combined_features.best_match();
        cout << "    *** Final Classification: "<< *hypothesis << "."<< endl;
        cout << "    *** Belief: " << combined_features.belief(&(*hypothesis),NULL) << ", Plausability: " << combined_features.plausability(&(*hypothesis),NULL) << ".\n\n";
    }
}

/**
 * @brief PSpaceIndex::getRules gets all rules in this index
 * @return all rules
 */
set<Nugget*> *PSpaceIndex::getNuggets() {
    return &allRules;
}

/**
 * @brief PSpaceIndex::getRules gets all rules supporting the hypothesis
 * with name = hypothesisName.
 * @param hypothesisName the name of the hypothesis
 * @return all rules which support that hypothesis
 */
set<Nugget*> *PSpaceIndex::getNuggets(string hypothesisName) {
    return hypothesisRules[hypothesisName];
}

/**
 * @brief PSpaceIndex::addHypothesisObject attempts to add a hypothesis object representing the
 * given string to the given set. If one already exists in the given set then it returns the
 * existing object.
 * @param hypothesisName the string to add
 * @param existingHypotheses the set of hypothesis obejcts
 * @return an object representing the given hypothesis
 */
void *PSpaceIndex::addHypothesisObject(string hypothesisName, set<void*>*existingHypotheses){
    //iterate through set
    for(set<void*>::iterator it = existingHypotheses->begin(); it != existingHypotheses->end(); ++it) {
        if(hypothesisName.compare(*((string*)*it)) == 0) {
            cout << "      (Found '" << hypothesisName << "', returning)." << endl;
            return *it;
        }
    }
    cout << "      (Failed to find '" << hypothesisName << "', creating)."<<endl;
    void* temp = (void*) new string(hypothesisName);
    existingHypotheses->insert(temp);
    return temp;
}


