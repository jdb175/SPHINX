#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QDesktopWidget>
#include <map>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <limits>
#include "util/json/json.h"

#include "graphical/GUI.hpp"
#include "graphical/IndexUpdateEvent.hpp"
#include "graphical/CreateIndexDialog.h"
#include "graphical/ruleview/RuleView.hpp"
#include "paras/PARAS.hpp"
#include "graphical/PSpaceGraph.hpp"
#include "graphical/Nugget.hpp"
#include "graphical/EventController.hpp"
#include "util/Utility.hpp"

#define ICON_PARAS QIcon(QString::fromUtf8("PARAS.png"));


using namespace PARASProgram;
using namespace PARASProgram::Graphical;

struct compPointer {
    bool operator() (Nugget* i,Nugget* j) { return (*i<*j);}
} compP;

struct compXY {
    bool operator() (XYPair* i,XYPair* j) {
        return (i->getX() < j->getX() || (i->getY() < j->getY() && i->getX() == j->getX()));
    }
} compx;

GUI::GUI()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "SPHINX", "SPHINX");
    configPath = settings.fileName().toStdString();

    haveConfigFile = fstream(configPath.c_str()).good();
    if(!haveConfigFile) settings.setValue("Creating","File");

    pInstance = new PARAS();
    graphInstance = NULL;
    rulePanel = NULL;
    evCont = new EventController(this);
    granularityIndexMap = new map<float, IndexUpdateEvent>();
    stableRegionRulesALL = new map<XYPair*, set<Rule*>*>();
    stableRegionRulesUNIQUE = new map<XYPair*, set<Rule*>*>();
    stableRegionRulesALL_NR = new map<XYPair*, set<Rule*>*>();
    uniqueRulesLists = new vector< set<Rule*> >();
    truncateVal = 50;

    colorManage = new ColorManager();
    colorMappings = new ColorMap(colorManage->getPredefinedColorRamp(ColorManager::SEQ_ORANGES));
    colorSelect = new ColorSelector(this, colorManage, colorMappings);
}

/**
 * @brief GUI::start initializes the GUI application.
 */
void GUI::start()
{

    centralWidget = new QWidget(this);
    
    //Get desktop size for scaling window
    QDesktopWidget *desktop = QApplication::desktop();
    
    //build the PSpaceGraph
    graphInstance = new PSpaceGraph(centralWidget, evCont, colorMappings, colorSelect);
    evCont->registerListener(graphInstance);
    
    //build layout for GUI.
    line = new QFrame(centralWidget);
    line->setFrameShape(QFrame::VLine);
    line->setFrameShadow(QFrame::Sunken);
    
    rulePanel = new RuleView(centralWidget, evCont, colorMappings);
    evCont->registerListener(rulePanel);
    
    evCont->setRuleMode(RuleMode(UNIQUE));

    //build the main window.
    this->resize(desktop->width(), desktop->height());
    this->setWindowTitle("PARAS GUI");
    //this->setWindowIcon(ICON_PARAS);
    this->setCentralWidget(centralWidget);
    this->setMenuBar(buildMenuBar());
    this->setMinimumHeight(600);
    this->setMinimumWidth(1000);

    loadConfigInfo();
    this->show();
}

void GUI::loadConfigInfo()
{
    cout << "Loading Config Info\n";
    Json::Value root;
    Json::Reader reader;

    ifstream config(configPath.c_str());

    //read all of config file.
    string configContents;
    if (config)
    {
        config.seekg(0, std::ios::end);
        configContents.resize(config.tellg());
        config.seekg(0, std::ios::beg);
        config.read(&configContents[0], configContents.size());
        config.close();
    }

    bool parsed = reader.parse(configContents, root);

    if(!parsed) return;

    //load glyph coloring
    int antB = root["color_map"]["glyph"]["antecedent"].get("blue", 105).asInt();
    int antG = root["color_map"]["glyph"]["antecedent"].get("green", 0).asInt();
    int antR = root["color_map"]["glyph"]["antecedent"].get("red", 0).asInt();

    int consB = root["color_map"]["glyph"]["consequent"].get("blue", 0).asInt();
    int consG = root["color_map"]["glyph"]["consequent"].get("green", 255).asInt();
    int consR = root["color_map"]["glyph"]["consequent"].get("red", 0).asInt();

    int conB = root["color_map"]["glyph"]["connect"].get("blue", 164).asInt();
    int conG = root["color_map"]["glyph"]["connect"].get("green", 160).asInt();
    int conR = root["color_map"]["glyph"]["connect"].get("red", 160).asInt();

    int misB = root["color_map"]["glyph"]["missing"].get("blue", 164).asInt();
    int misG = root["color_map"]["glyph"]["missing"].get("green", 0).asInt();
    int misR = root["color_map"]["glyph"]["missing"].get("red", 160).asInt();

    int ramp = root["color_map"].get("ramp", 2).asInt();

    colorMappings->glyphAntecedentColor.setRed(antR);
    colorMappings->glyphAntecedentColor.setGreen(antG);
    colorMappings->glyphAntecedentColor.setBlue(antB);

    colorMappings->glyphConsequentColor.setRed(consR);
    colorMappings->glyphConsequentColor.setGreen(consG);
    colorMappings->glyphConsequentColor.setBlue(consB);

    colorMappings->glyphConnectColor.setRed(conR);
    colorMappings->glyphConnectColor.setGreen(conG);
    colorMappings->glyphConnectColor.setBlue(conB);

    colorMappings->glyphMissingColor.setRed(misR);
    colorMappings->glyphMissingColor.setGreen(misG);
    colorMappings->glyphMissingColor.setBlue(misB);

    colorMappings->setColorRamp(this->colorManage->getPredefinedColorRamp(ramp));

    //load graph settings.
    int granularity = root.get("granularity", 50.0).asDouble();
    bool redun = root.get("redundancies", false).asBool();
    int ruleMode = root.get("rule_mode", 0).asInt();

    bool gridLines = root["graph"].get("grid_lines", true).asBool();
    bool skyline = root["graph"].get("skyline", true).asBool();
    int skylineCardinality = root["graph"].get("skyline_cardinality_value", 0).asInt();

    evCont->setRuleMode(RuleMode(ruleMode));
    this->truncateVal = granularity;
    evCont->updateRedundancy(redun);

    graphInstance->setConfig(gridLines, skyline, redun, RuleMode(ruleMode), skylineCardinality,granularity);

    //load default file.
    string defaultFile = root.get("default_file", "").asString();
    bool successfullyLoaded = pInstance->loadFile(defaultFile);
    if(successfullyLoaded) updateForLoadedIndex(QString::fromStdString(defaultFile));
}

void GUI::resizeEvent(QResizeEvent *e)
{
    line->move(e->size().width()/2 - 25,0);
    line->resize(3,e->size().height());
    evCont->applicationSizeChanged(e->size().width(), e->size().height());
}

/**
 * @brief GUI::buildMenuBar
 * @return The built menu bar for the PARAS gui.
 */
QMenuBar *GUI::buildMenuBar()
{
    QMenuBar *menu = new QMenuBar();
    
    
    QMenu *pspace = new QMenu("PSpaceIndex");
    
    QAction *open = new QAction("Create New Index", this);
    QObject::connect(open, SIGNAL(triggered()), this, SLOT(createPSpaceIndex()));
    
    QAction *openEx = new QAction("Open Existing Index", this);
    QObject::connect(openEx, SIGNAL(triggered()), this, SLOT(loadExistingIndex()));
        
    close = new QAction("Close Index", this);
    QObject::connect(close, SIGNAL(triggered()), this, SLOT(closeIndex()));
    
    pspace->addAction(open);
    pspace->addAction(openEx);
    pspace->addAction(close);
    
    
    QMenu *help = new QMenu("Help");
    
    QAction *about = new QAction("About", this);
    QAction *helpAction = new QAction("Help", this);
    
    help->addAction(helpAction);
    help->addAction(about);
    
    
    menu->addMenu(pspace);
    menu->addMenu(help);
    return menu;
}


/**
 * @brief GUI::updateForLoadedIndex updates the information throughout GUI based on the active GUI
 * @param fileChosen
 */
void GUI::updateForLoadedIndex(QString fileChosen)
{
    if(granularityIndexMap->count(truncateVal) == 1)
    {
        evCont->updateIndexInfo(granularityIndexMap->at(truncateVal));
        return;
    }

    vector<XYPair*> *sPoints = pInstance->getActiveStableRegionPoints();


    if(fileChosen != NULL)
    {
        for(vector<XYPair*>::iterator i = sPoints->begin(); i != sPoints->end(); ++i)
        {
            set<Rule*> *allRules_nr = pInstance->getRulesForStableRegionPoint((*i),false,false);
            set<Rule*> *uRules = pInstance->getRulesForStableRegionPoint((*i),true,true);
            set<Rule*> *allRules = pInstance->getRulesForStableRegionPoint((*i),false,true);

            stableRegionRulesALL_NR->insert(std::pair<XYPair*,set<Rule*>*>((*i), allRules_nr));
            stableRegionRulesALL->insert(std::pair<XYPair*,set<Rule*>*>((*i), allRules));
            stableRegionRulesUNIQUE->insert(std::pair<XYPair*,set<Rule*>*>((*i), uRules));
        }
    }



    vector<Nugget*> *stableRegionPoints = new vector<Nugget*>;
    vector<Nugget*> truncatedRegions;


    Nugget *s;

    set<Rule*> *uRules;
    set<Rule*> *allRules;
    set<Rule*> *allRulesNR;

    
    float sup;
    float conf;
    
    int maxRulesU = 0;
    int minRulesU = std::numeric_limits<int>::max();
    int maxRulesA = 0;
    int minRulesA = std::numeric_limits<int>::max();

    int maxRulesU_nr = 0;
    int minRulesU_nr = std::numeric_limits<int>::max();
    int maxRulesA_nr = 0;
    int minRulesA_nr = std::numeric_limits<int>::max();

    
    float minConf = 1;
    float minSup = 1;
    float maxSup = 0;
    float maxConf = 0;
    
    float truncateSup;
    float truncateConf;
    float lastSup = -1;
    float lastConf = -1;
    Nugget* lastS;

    //Runtime granularity control
    //Combine the regions into truncateVal number of bins, to make them more easily visible

    //First iterate through and truncate values
    for(std::vector<XYPair*>::size_type i = 0; i != sPoints->size(); i++)
    {
        conf = sPoints->at(i)->getY();
        sup = sPoints->at(i)->getX();

        set<Rule*> *uniqueRulesCur;
        set<Rule*> *uniqueRulesCurNR;

        uRules = (*stableRegionRulesUNIQUE)[sPoints->at(i)];
        uniqueRulesCur = new set<Rule*>(uRules->begin(), uRules->end());
        uniqueRulesCurNR = new set<Rule*>(uRules->begin(), uRules->end());

        uniqueRulesLists->push_back(*uniqueRulesCur);


        allRules = (*stableRegionRulesALL)[sPoints->at(i)];
        allRulesNR = (*stableRegionRulesALL_NR)[sPoints->at(i)];

        truncateSup = floor(sup * truncateVal) / truncateVal;
        truncateConf = floor(conf * truncateVal) / truncateVal;

        //Create a temporary region with the truncated parameters
        s = new Nugget (colorMappings, truncateSup, truncateConf, allRules, uniqueRulesCur, allRulesNR, uniqueRulesCurNR);
        truncatedRegions.push_back(s);
    }


    //Resort the regions list, since the truncation breaks the original sorting
    std::sort(truncatedRegions.begin(),truncatedRegions.end() ,compP);


    //Now merge any redundant regions created by the truncation, so that we don't lose any rules
    for(std::vector<Nugget*>::size_type i = 0; i != truncatedRegions.size(); i++)
    {

        conf = truncatedRegions[i]->confidence;
        sup = truncatedRegions[i]->support;

        //Get useful parameter interval
        minConf = std::max(0.0f, std::min(conf-1/truncateVal, minConf));
        maxConf = std::min(1.0f, std::max(conf, maxConf));
        minSup = std::max(0.0f,  std::min(sup-1/truncateVal, minSup));
        maxSup = std::min(1.0f, std::max(sup, maxSup));

        //now combine regions within same bin by checking against last values
        if(sup != lastSup || conf != lastConf)
        {
            //If this is a distinct region, add it to the list
            lastS = truncatedRegions[i];
            stableRegionPoints->push_back(lastS);
        }
        else
        {
            //Otherwise just merge it into the last region

            //But first we handle redundancies
            set<Rule*> *rulesToAdd = new set<Rule*>;
            for(set<Rule*>::iterator j = truncatedRegions[i]->uniqueRules->begin(); j != truncatedRegions[i]->uniqueRules->end(); ++j)
            {
                Rule *rule = *j;

                //if the rule has a dominating point, it can only be added as a rule if its dominating point's support or confidence
                //do not fall within this truncated region. Otherwise, its dominant point should be added instead later on.
                if ((rule->getDominantPointSimple() == NULL) && (rule->getDominantPointStrict() == NULL))
                {
                    rulesToAdd->insert(rule);
                }
                else if((rule->getDominantPointSimple() == NULL) && (rule->getDominantPointStrict() != NULL))
                {
                    if (rule->getDominantPointStrict()->getX() <= sup-1/truncateVal || rule->getDominantPointStrict()->getY() <= conf-1/truncateVal) rulesToAdd->insert(rule);
                }
                else if ((rule->getDominantPointSimple() != NULL) && (rule->getDominantPointStrict() == NULL))
                {
                    if ((rule->getDominantPointSimple()->getX() <= sup-1/truncateVal) || (rule->getDominantPointSimple()->getY() <= conf-1/truncateVal)) rulesToAdd->insert(rule);
                }
                else  if (((rule->getDominantPointSimple()->getX() <= sup-1/truncateVal) || (rule->getDominantPointSimple()->getY() <= conf-1/truncateVal))
                          && ((rule->getDominantPointStrict()->getX() <= sup-1/truncateVal) || (rule->getDominantPointStrict()->getY() <= conf-1/truncateVal))){
                    rulesToAdd->insert(rule);
                }
            }


            lastS->uniqueRules->insert(truncatedRegions[i]->uniqueRules->begin(), truncatedRegions[i]->uniqueRules->end());
            lastS->uniqueRules_nr->insert(rulesToAdd->begin(), rulesToAdd->end());

            if(truncatedRegions[i]->allRules->size() > lastS->allRules->size())
            {
                lastS->allRules = truncatedRegions[i]->allRules;
            }

            if(truncatedRegions[i]->allRules_nr->size() > lastS->allRules_nr->size())
            {
                lastS->allRules_nr = truncatedRegions[i]->allRules_nr;
            }

            //And now delete it, since it has no purpose
            delete truncatedRegions[i];
        }
        lastSup = sup;
        lastConf = conf;

        //get min and max rules
        maxRulesU = std::max((int)lastS->uniqueRules->size(), maxRulesU);
        minRulesU = std::min((int)lastS->uniqueRules->size(), minRulesU);
        maxRulesA = std::max((int)lastS->allRules->size(), maxRulesA);
        minRulesA = std::min((int)lastS->allRules->size(), minRulesA);

        maxRulesU_nr = std::max((int)lastS->uniqueRules_nr->size(), maxRulesU_nr);
        minRulesU_nr = std::min((int)lastS->uniqueRules_nr->size(), minRulesU_nr);
        maxRulesA_nr = std::max((int)lastS->allRules_nr->size(), maxRulesA_nr);
        minRulesA_nr = std::min((int)lastS->allRules_nr->size(), minRulesA_nr);

    }


    IndexUpdateEvent newIndexLoaded;
    newIndexLoaded.allInterval = pair<int,int>(minRulesA, maxRulesA);
    newIndexLoaded.allInterval_nr = pair<int,int>(minRulesA_nr, maxRulesA_nr);
    newIndexLoaded.confInterval = pair<double,double>(minConf, maxConf);
    newIndexLoaded.supInterval = pair<double,double>(minSup, maxSup);
    newIndexLoaded.stableRegions = stableRegionPoints;
    newIndexLoaded.uniqueInterval = pair<int,int>(minRulesU, maxRulesU);
    newIndexLoaded.uniqueInterval_nr = pair<int,int>(minRulesU_nr, maxRulesU_nr);
    newIndexLoaded.attributes = pInstance->getAttributes();

    granularityIndexMap->insert(std::pair<float,IndexUpdateEvent>(truncateVal, newIndexLoaded));
    evCont->updateIndexInfo(newIndexLoaded);

    if(fileChosen != NULL)
    {
        this->setWindowTitle(QString("PARAS GUI - ").append(fileChosen));
        this->close->setEnabled(true);
    }
}

/**
 * @brief GUI::updateGranularity updates the granularity throughout the program.
 * @param g : granularity value (number of bins)
 */
void GUI::updateGranularity(int g)
{
    if(truncateVal == g) return;
    truncateVal = g;
    if(pInstance->indexOpen())
    {
        updateForLoadedIndex(NULL);
    }
}

/**
 * @brief GUI::updateRedundancy updates the redundancy throughout the program.
 * @param r redundancies included?
 */
void GUI::updateRedundancy(bool r) {
    if(redundancies == r) return;
    redundancies = r;
    if(pInstance->indexOpen())
    {
        updateForLoadedIndex(NULL);
    }
}


/*
 * SLOTS
 */

/**
 * @brief GUI::onQuit performs clean up operations when the program is quitting, such as saving configurations.
 */
void GUI::onQuit()
{
    cout << "Saving Config Info" << endl;

    Json::Value root;
    Json::StyledWriter writer;

    root["default_file"] = pInstance->getActiveFileName();

    root["rule_mode"] = this->graphInstance->ruleMode;
    root["redundancies"] = this->redundancies;
    root["granularity"] = this->truncateVal;
    root["graph"]["grid_lines"] = this->graphInstance->bgLines;
    root["graph"]["skyline"] = this->graphInstance->skyline;
    root["graph"]["skyline_cardinality_value"] = this->graphInstance->skyLineCardinality;

    root["color_map"]["ramp"] = this->colorMappings->getColorRamp()->getID();

    root["color_map"]["glyph"]["antecedent"]["red"] = this->colorMappings->glyphAntecedentColor.red();
    root["color_map"]["glyph"]["antecedent"]["green"] = this->colorMappings->glyphAntecedentColor.green();
    root["color_map"]["glyph"]["antecedent"]["blue"] = this->colorMappings->glyphAntecedentColor.blue();

    root["color_map"]["glyph"]["consequent"]["red"] = this->colorMappings->glyphConsequentColor.red();
    root["color_map"]["glyph"]["consequent"]["green"] = this->colorMappings->glyphConsequentColor.green();
    root["color_map"]["glyph"]["consequent"]["blue"] = this->colorMappings->glyphConsequentColor.blue();

    root["color_map"]["glyph"]["connect"]["red"] = this->colorMappings->glyphConnectColor.red();
    root["color_map"]["glyph"]["connect"]["green"] = this->colorMappings->glyphConnectColor.green();
    root["color_map"]["glyph"]["connect"]["blue"] = this->colorMappings->glyphConnectColor.blue();

    root["color_map"]["glyph"]["missing"]["red"] = this->colorMappings->glyphMissingColor.red();
    root["color_map"]["glyph"]["missing"]["green"] = this->colorMappings->glyphMissingColor.green();
    root["color_map"]["glyph"]["missing"]["blue"] = this->colorMappings->glyphMissingColor.blue();


    string outputConfig = writer.write( root );
    cout << configPath << endl;
    ofstream configOut;
    configOut.open(configPath.c_str());
    configOut << outputConfig;
    configOut.close();
}

/**
 * @brief GUI::createPSpaceIndex loads a raw .txt data file that has already been put through the apriori algorithm.
 */
void GUI::createPSpaceIndex()
{
    CreateIndexDialog dialg(this, pInstance);
    graphInstance->setCursorHiding(false);
    dialg.exec();
    graphInstance->setCursorHiding(true);
    if(dialg.result() == 1 && *dialg.GetFileName() != "" && fstream((*dialg.GetFileName()).c_str()).good()) {
        closeIndex();
        pInstance->loadExistingPSpaceIndex(*dialg.GetFileName());
        updateForLoadedIndex(QString::fromStdString(*dialg.GetFileName()));
    }
}

/**
 * @brief GUI::loadExistingIndex loads .psi files of pre-computed indices.
 */
void GUI::loadExistingIndex()
{
    QString fileChosen = QFileDialog::getOpenFileName(this, QString("Open Existing PSpaceIndex File."), QString(), QString("*.psi"));
    if(fileChosen == NULL) return;
    closeIndex();
    pInstance->loadExistingPSpaceIndex(fileChosen.toStdString());
    
    updateForLoadedIndex(fileChosen);
}

/**
 * @brief GUI::closeIndex closes the current index and frees all memory used by it.
 */
void GUI::closeIndex()
{
    for(map<float, IndexUpdateEvent>::const_iterator it=granularityIndexMap->begin(); it != granularityIndexMap->end(); ++it)
    {
        IndexUpdateEvent e = it->second;

        for(size_t i = 0; i < e.stableRegions->size(); i++) (e.stableRegions->at(i))->fullDelete();

        delete e.stableRegions;
    }

    //clear the caching done
    granularityIndexMap->clear();
    stableRegionRulesALL->clear();
    stableRegionRulesUNIQUE->clear();
    stableRegionRulesALL_NR->clear();

    //delete previous allocated copied rules lists.
    for(size_t i = 0; i < uniqueRulesLists->size(); i++)
    {
        uniqueRulesLists->at(i).clear();
    }

    uniqueRulesLists->clear();

    //close the index in PARAS, which will free all memory from the PSPaceIndex.
    pInstance->closeIndex();
    evCont->closeIndex(); //signal to other GUI elements to close the index.
    //save->setEnabled(false);
    close->setEnabled(false);
    this->setWindowTitle("PARAS GUI");
}

/**
 * @brief GUI::saveIndex brings up the file dialog to save the active index.
 */
void GUI::saveIndex()
{
    QString fileChosen = QFileDialog::getSaveFileName(this, QString("Save PSpaceIndex to File."), QString(), QString("*.psi"));
    if(fileChosen == NULL) return;
    pInstance->saveActiveIndex(fileChosen.toStdString(), false);
    this->setWindowTitle(QString("PARAS GUI - ").append(fileChosen));
}

void GUI::keyPressEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_Shift) graphInstance->secondarySelect = true;
}

void GUI::keyReleaseEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_Shift) graphInstance->secondarySelect = false;
}
