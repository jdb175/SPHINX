// paras.cpp: main project file & entry point.

#include <fstream>
#include <string>
#include <vector>

#include "PreProcessor.hpp"
#include "paras/PARAS.hpp"
#include "util/Utility.hpp"
#include "paras/PSpaceIndex.hpp"
#include "util/Convert.hpp"
#include "apriori/Apriori.hpp"

using namespace std;
using namespace SPHINXProgram;

/**
 * Constructor.
 */
PARAS::PARAS()
{
    gene = new PSpaceGenerator();
    activeIndex = NULL;
}


bool PARAS::createIndex(string iFileName, string oFileName, string delimiter, double minSup, double minConf, unsigned long sizeThreshold, double precision, bool compress, bool noQuotes){

    //preprocess csv data
    PreProcessor pre(iFileName, delimiter, minSup, minConf, sizeThreshold, precision, noQuotes);
    string tempFile = pre.getProcessedFile();

    //create an index
    loadNewPSpaceIndex(tempFile);
    activeIndex->setFileName(new string(oFileName));
    saveActiveIndex(oFileName, compress);

    return true;
}


bool PARAS::loadFile(string fileName)
{
    //check if file can be opened.
    fstream testFile(fileName.c_str());

    if(!testFile.is_open()) return false;

    string ext = fileName.substr(fileName.length() - 4, fileName.npos);

    if(ext.compare(".psi") == 0 || ext.compare(".PSI") == 0)
    {
        loadExistingPSpaceIndex(fileName);
    }
    else
    {
        //load the index
        loadNewPSpaceIndex(fileName);
        activeIndex->setFileName(new string(fileName));
    }

    return true;
}

void PARAS::loadNewPSpaceIndex(string fileName)
{
    string *file = new string(fileName);
    //pspaceindex does not exist yet, so build it.
    PSpaceIndex *forFile = new PSpaceIndex(file,false);
    gene->generatePSpaceIndex(forFile);
    activeIndex = forFile;
}

void PARAS::loadExistingPSpaceIndex(string fileName)
{
    PSpaceIndex *forFile = new PSpaceIndex(new string(fileName), true);
    activeIndex = forFile;
}

void PARAS::saveActiveIndex(string fileName, bool compressed)
{
    activeIndex->savePSpaceIndexToFile(fileName, compressed);
}

bool PARAS::indexOpen() {
    return (activeIndex != NULL);
}

void PARAS::closeIndex()
{
    if(!indexOpen()) return;
    delete activeIndex;
    activeIndex = NULL;
}

/**
 * Query the active pspaceindex.
 * @return vector of rules or NULL if there is no active pspaceindex.
 */
set<Rule*> *PARAS::queryActiveIndex(double support, double confidence, bool redun)
{
    if(activeIndex == NULL) return NULL;

    return activeIndex->findRulesForSupportConfidence(support, confidence, redun);
}

vector<XYPair*> *PARAS::getActiveStableRegionPoints()
{
    return activeIndex->getStableRegionPoints();
}

set<Rule*> *PARAS::getRulesForStableRegionPoint(XYPair *sr, bool unique, bool redun)
{
    if(unique)
    {
        return activeIndex->getRulesForStableRegion(sr);
    }
    else
    {
        return queryActiveIndex(sr->getX(), sr->getY(), redun);
    }
}

vector<Attribute*> *PARAS::getAttributes()
{
    return activeIndex->getAttributeList();
}

string PARAS::getActiveFileName()
{
    if(activeIndex == NULL) return "";

    return *activeIndex->getFileName();
}

