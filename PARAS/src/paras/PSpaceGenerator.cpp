#include <iostream>
#include <string>
#include <cstdlib>
#include <map>
#include <math.h>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <vector>

#include "paras/PSpaceGenerator.hpp"
#include "paras/PSpaceIndex.hpp"
#include "util/Utility.hpp"
#include "util/Convert.hpp"

using namespace std;
using namespace PARASProgram;


struct rComp
{
    bool operator()(Rule *i, Rule *j)
    {
        double lFrontSupp = i->getSupport();
        double rFrontSupp = j->getSupport();
        double rFrontConf = j->getConfidence();
        double lFrontConf = i->getConfidence();
        bool lLess = lFrontSupp < rFrontSupp;

        if(lFrontSupp == rFrontSupp) lLess = lFrontConf < rFrontConf;

        return lLess;
    }
} ruleComp;

/**
 * Initialize the rule generator
 */
void PSpaceGenerator::initializePSpaceGenerator()
{
	strongRulesList = new vector<Rule*>();
    domain = new set<string>();
	frequentItemsDictionary = new map<string, double>();
    totalOccurences = 0;
    mapped = true;
}

/**
 * Start the rule generating process for the target pspaceindex.
 */
void PSpaceGenerator::generatePSpaceIndex(PSpaceIndex *targetIndex)
{
	if(targetIndex == NULL)
	{
		cout << "Error: PSpaceIndex null during rule generation." << endl;
		return;
	}

	activeIndex = targetIndex;
	strongRulesList->clear();
	domain->clear();
    totalOccurences = 0;
    mapped = true;

    loadFile(targetIndex->getFileName());
    cout << "Domain Size: " << domain->size() << endl;
	findStableRegions();
	activeIndex->preStoreRules(storeRules());
}

/**
 * Loads the rules from the specified file.  Includes loading the item sets and rules.
* @param fileName the name of the file to load the rules from.
* @return the number of attributes in the file.
*/
void PSpaceGenerator::loadFile(string *fileName)
{
	//attempt to open the file.
	cout << "Opening File: " << *fileName << endl;
	ifstream fileIn;
	fileIn.open(fileName->c_str(), ifstream::in);

	if(!fileIn.is_open())
	{
		cout << "Error opening file.  Exiting." << endl;
	}

	//start a count of the item sets loaded.
	int itemSetCount = 0;
	string readStr;
	bool isItemSets = true;

	while (fileIn.good() && !fileIn.eof()) //read each line of file till end of file.
	{
		getline(fileIn, readStr);
        if(readStr.compare("Attribute Map:") == 0)
        {
            readAttributes(&fileIn);
            continue;
        }

        if (readStr.compare("condition ==>consequence (confidence, occurrence)") == 0) //check if pattern is not item set.
		{
			isItemSets = false;
		}

		// load itemsets if they exist in this line.
		if (isItemSets)
		{
			//if match is found, increment the item set count.
			//load the item set.
			if (Utility::startsWithDigit(readStr))
			{
				readItemSetsFromFile(&readStr);
				itemSetCount++;
			}
            else if(readStr.substr(0,2).compare("{}") == 0)
            {
                readStr = readStr.substr(3,string::npos);

                readStr.erase(std::remove(readStr.begin(), readStr.end(), '('), readStr.end());
                readStr.erase(std::remove(readStr.begin(), readStr.end(), ')'), readStr.end());
                totalOccurences = Convert::stringToDouble(&readStr);
                cout << "Total Itemset Occurences " << totalOccurences << endl;
            }

        }
		else
		{ //if not an item set, then have loaded all the item sets within the rules file.
            cout << "Loaded Itemsets: " << itemSetCount << endl;
			readRulesFromFile(&fileIn);  // read all the rules
			break;  // finish file reading
		}
	}
	fileIn.close();  // close file
}

/**
 * @brief PSpaceGenerator::readAttributes reads the atttributes in from the input stream to map the values to specific attributes.
 * @param din
 */
void PSpaceGenerator::readAttributes(ifstream *din)
{
    string str;

    if((*din).is_open())
    {
        while((*din).good() && !(*din).eof())
        {
            getline((*din),str);
            if(str.size() > 0 && Utility::startsWithDigit(str))
            {
                //find out two sections
                size_t splitval = str.find("(");

                string encodings = str.substr(0, splitval);
                string values = str.substr(splitval+1);
                size_t splitval2 = values.find_last_of(")");
                values = values.substr(0, splitval2);

                //split the string at spaces
                stringstream splitstring(encodings);
                vector<string> *lst = new vector<string>();
                string* tmpstr = new string();
                while(getline(splitstring, *tmpstr, ' '))
                {
                    if(tmpstr->at(0) == '(') break;
                    lst->push_back(*tmpstr);
                    tmpstr = new string();
                }

                string *tmpTwo = new string();
                stringstream splitstring2(values);
                bool name = true;
                string attrName;
                vector<string> *nameList = new vector<string>();
                while(getline(splitstring2, *tmpTwo,','))
                {
                    if(name)
                    {
                        attrName = *tmpTwo;
                        name = false;
                    }
                    else
                    {
                        nameList->push_back(*tmpTwo);
                    }

                    tmpTwo = new string();
                }
                delete tmpstr;
                delete tmpTwo;

                Attribute *newAttr = new Attribute(new string(attrName),lst, nameList);
                newAttr->mapped = &mapped;
                activeIndex->addAttribute(newAttr);
                mapped &= nameList->size() > 0;
            }
            else
            {
                return;
            }
        }
    }
}

/**
 * Read in the rules from the specified input stream.
 * @param din the stream reader to read from.
 */
void PSpaceGenerator::readRulesFromFile(ifstream *din)
{
	// stop at the title
	string str;
	int count = 0;
	if((*din).is_open()) //check status of received file.
	{
		//continue going through the stream until hit the end of the file.
		while ((*din).good() && !(*din).eof())//(str = din->ReadLine()) != nullptr)
		{
			getline((*din), str);
			count++; //increment the count of rules
            if (str.size() > 0 && Utility::startsWithDigit(str))
			{	//if not a blank line, extract the numbers from the string and put them in a rule.
				buildRuleFromStr(str);
			}
		}
        cout << "Possible Loaded Rules: " << count << endl;
	}
	else
	{
		cout << "Error reading rules from file" << endl;
		exit(122);
	}
}

/**
 * Load the item sets within the string.
 * @param the string to load from.
 * @return the highest item set in the item set line, for use in finding the domain.
 */
void PSpaceGenerator::readItemSetsFromFile(string* str)
{
		//split the string at spaces
		stringstream splitstring(*str);
		vector<string*> strar;
		string *tmpstr = new string();
		while(getline(splitstring, *tmpstr, ' '))
		{
			strar.push_back(tmpstr);
			tmpstr = new string();
		}

		delete tmpstr;
		vector<string*> *itemsets = new vector<string*>(strar); //create itemsets containing the contents of strar.

		stringstream temp_itemstr;
        string* itemstr = new string();

		//check size of strar
		if (strar.size() > 2)
		{
			itemsets = Utility::numericalOrder(itemsets, true);
		}

		for (unsigned int i=0; i < itemsets->size() - 1; i++)
		{
			string* curItem = itemsets->at(i);
            temp_itemstr << *curItem;
            if(i != itemsets->size() - 2) temp_itemstr << ",";
		}

		*itemstr = temp_itemstr.str();

		string supportStr;

		supportStr = *strar.at(strar.size() - 1);
		supportStr.erase(std::remove(supportStr.begin(), supportStr.end(), '('), supportStr.end());
		supportStr.erase(std::remove(supportStr.begin(), supportStr.end(), ')'), supportStr.end());

        double sup = Convert::stringToDouble(&supportStr) / totalOccurences;
		frequentItemsDictionary->insert(std::pair<string,double>(*itemstr, sup));
        itemsets->clear();
        delete itemsets;
		delete itemstr;
}

/**
 * Builds a rule from the designated string
 * @param str the string to extract the rule from
 */
void PSpaceGenerator::buildRuleFromStr(string str)
{

	//split the string at spaces
	stringstream splitstring(str);
	vector<string*> lst;
	string* tmpstr = new string();

	while(getline(splitstring, *tmpstr, ' '))
	{
		lst.push_back(tmpstr);
		tmpstr = new string();
	}

	delete tmpstr;

	if (str.size() > 0)
	{
		int k = 0;

		vector<string*> *lst1 = new vector<string*>;
		vector<string*> *lst2 = new vector<string*>;

		for(std::vector<string*>::size_type i = 0; i != lst.size(); i++)
		{
			if (Utility::bothTrim(lst.at(i))->compare("==>") == 0)
				break;

            string* curItem = Utility::bothTrim(lst.at(i));
            domain->insert(*curItem);
            lst1->push_back(curItem);
			k++;
		}

        k++;

		for (unsigned int i = k; i < lst.size(); i++)
		{
			if (Utility::bothTrim(lst.at(i))->substr(0,1).compare("(") == 0) break;
            string* curItem = Utility::bothTrim(lst.at(i));
            domain->insert(*curItem);
            lst2->push_back(curItem);
			k++;
		}

        string *tmp = Utility::bothTrim(lst.at(k));
		string *s3 = tmp;
        size_t pos;
		//Replace("(","");
		while ((pos=s3->find("(")) != std::string::npos)
			s3->replace(pos, 1, "");
		//Replace(",","");
		while ((pos=s3->find(",")) != std::string::npos)
			s3->replace(pos, 1, "");
		k++;

        tmp = Utility::bothTrim(lst.at(k));
		string *s4 = tmp;
		//Replace(")","");
		while ((pos=s4->find(")")) != std::string::npos)
			s4->replace(pos, 1, "");

		vector<string*> *a1 = Utility::numericalOrder(lst1, false);
        vector<string*> *a2 = lst2;

        double d4 = Convert::stringToDouble(s4) / totalOccurences;
        double d3 = Convert::stringToDouble(s3);
		Rule* NewRule = new Rule(a1, a2, d4, d3, NULL);
		strongRulesList->push_back(NewRule);

	}
}

/**
 * Processes all of the rules found in the file and create pairs of points for each.
 * Then identifies the stable regions from these points and designates them as so.
 */
void PSpaceGenerator::findStableRegions()
{
	cout << "Building stable regions." <<endl;

    std::sort(strongRulesList->begin(), strongRulesList->end(), ruleComp);

	vector<XYPair*> *pSpacePoints = new vector<XYPair*>();

	XYPair *SuppConf; //a pair to hold the support and confidence of each rule in the strong rules list.
	//create a point for each strong rule.
	for(unsigned int i = 0; i < strongRulesList->size(); i++)
	{
		Rule *ar = strongRulesList->at(i);
		SuppConf = new XYPair(ar->getSupport(), ar->getConfidence());
		pSpacePoints->push_back(SuppConf); //add each point.
	}
	activeIndex->setPSpacePoints(pSpacePoints);
    vector<XYPair*> *sRegions = Utility::distinctPoints(pSpacePoints);
    activeIndex->setStableRegionPoints(sRegions); //separate out the distinct points.
	cout << "Building redundancies. This may take a few minutes." << endl;
	strongRulesList = buildRedund(); //construct redundancy relation
	cout << "Finished building redundancies." << endl;
}



/**
* Builds the redundancy index from the PSpaceIndex.
*/
vector<Rule*> *PSpaceGenerator::buildRedund()
{
	double support = 0;

	XYPair *pointSim;
	XYPair *pointStr;
	double supportStr = 0;
	double storedSupport = 0;

	string indexStr;
	int p = 0;
	int srlSize = strongRulesList->size();
	for (int i = 0; i < srlSize; i++)
	{
		supportStr = 0;
		support = 0;
		Rule* currentRule = strongRulesList->at(i);

		//below is for simple dominating point
		vector<string*> *strX = Utility::numericalOrder(currentRule->getX(), false);

		int strXSize = strX->size();
		if(strXSize > 1)
		{
			for(int k = 0; k < strXSize; k++)
			{

				vector<string*> strX_2;
				strX_2.insert(strX_2.begin(), strX->begin(), strX->begin() + p);
				strX_2.insert(strX_2.begin() + p, strX->begin() + p + 1, strX->end());

				indexStr = *Utility::arrayToString(&strX_2, &indexStr);
				p++;

				if(frequentItemsDictionary->count(indexStr) != 0)
				{
					storedSupport = frequentItemsDictionary->at(indexStr);
				}


				if(support == 0) support = storedSupport;
				if (support > storedSupport) support = storedSupport;
			}
		}
		p = 0;

		if(strXSize > 1)
		{

			pointSim = new XYPair(currentRule->getSupport(), currentRule->getSupport() / support);
			currentRule->setDominantPointSimple(pointSim);
		}

		//below is for strict dominating point

		//combine x and y strings into one.
		vector<string*> *strY = Utility::numericalOrder(currentRule->getY(),false); //sort Y string
		vector<string*> strXY;

		strXY.insert(strXY.end(), strX->begin(), strX->end());
		strXY.insert(strXY.end(), strY->begin(), strY->end());

		strXY = *Utility::numericalOrder(&strXY,false); //sort strXY

        for(set<string>::iterator j = domain->begin(); j != domain->end(); ++j)
		{
            string xc = (*j);
            string *x = &xc;

			if (!Utility::isSubstring(x, &strXY))
			{
				vector<string*> newXY;

				newXY.insert(newXY.begin(), strXY.begin(), strXY.end());

				newXY.push_back(x);

				newXY = *Utility::numericalOrder(&newXY,false); //sort the new xy

				indexStr = *Utility::arrayToString(&newXY, &indexStr);

				if(frequentItemsDictionary->count(indexStr) != 0)
				{
					storedSupport = frequentItemsDictionary->at(indexStr);
					if(storedSupport > supportStr) supportStr = storedSupport;
				}
			}
		}

		indexStr = *Utility::arrayToString(currentRule->getX(), &indexStr);
		pointStr = new XYPair(supportStr, supportStr / frequentItemsDictionary->at(indexStr));

		currentRule->setDominantPointStrict(pointStr);
	}

	return strongRulesList;
}

/**
 * Store the rules based on confidence and support to be indexed into later.
 * @param lstStrongRules the list of rules to be stored.
 */
map<string, set<Rule*>*> *PSpaceGenerator::storeRules()
{
	cout << "Storing rules." << endl;
    map<string, set<Rule*>*> *preRules = new map<string, set<Rule*>*>();
    int ruleCount = 0;
	string *threshold;
	long long threshold_long;

	double rSupport;
	double pastSupp = 0.0;
	double pastConf = 0.0;
	int finalSupport, finalConfidence;

	int pastIndex = 0;
	int currentIndex = 0;
	int size = strongRulesList->size();
	for(int i = 0; i < size; i++)
	{
		Rule *rule = strongRulesList->at(i);

		//translate the support and confidence into a string that can be used to index the rules.
		double supp = rule->getSupport();
		double conf = rule->getConfidence();
		rSupport = (supp / (double)(activeIndex->lastTransactionID)) * 100;

		finalSupport = (int)(rSupport * 1000);
		finalConfidence = (int)(conf * 1000000);

		threshold_long = (long long)(10000000 * pastSupp + pastConf);
		threshold = Convert::longToString(threshold_long);

		if ((currentIndex != 0) && ((finalConfidence != pastConf) || (finalSupport != pastSupp)))
		{
            set<Rule*> *copiedList = new set<Rule*>();
			//copy over the current rule to the preRules.
			for(int j = 0; j < currentIndex-pastIndex; j++)
			{
                copiedList->insert(strongRulesList->at(pastIndex + j));
			}

            preRules->insert(std::pair<string,set<Rule*>*>(*threshold,copiedList));
            ruleCount += copiedList->size();
			pastIndex = currentIndex;
		}

		delete threshold;
		pastSupp = finalSupport;
		pastConf = finalConfidence;
		currentIndex++;
	}

	long long temp = (long long)(10000000 * pastSupp + pastConf);

	threshold = Convert::longToString(temp);

    set<Rule*> *copiedList = new set<Rule*>();
	//copy over the current rule to the preRules.
	for(int i = 0; i < currentIndex-pastIndex; i++)
	{
        copiedList->insert(strongRulesList->at(pastIndex + i));
	}

    preRules->insert(std::pair<string,set<Rule*>*>(*threshold,copiedList));
    ruleCount += copiedList->size();

    cout << "Rules Saved: " << ruleCount << endl;
	delete threshold;
	return preRules;
}
