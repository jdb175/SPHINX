#ifndef PSPACEINDEX_H
#define PSPACEINDEX_H

#include <string>
#include <vector>
#include <map>
#include <set>
#include "paras/XYPair.hpp"
#include "paras/Rule.hpp"
#include "paras/Attribute.hpp"

using namespace std;

namespace PARASProgram
{
	class PSpaceIndex
	{
		public:
            PSpaceIndex(string *fileName, bool alreadyGenerated);
            ~PSpaceIndex();
			//public functions.
            set<Rule*> *findRulesForSupportConfidence(double minSupport, double minConfidence, bool withRedundancy);

            void savePSpaceIndexToFile(string file, bool compressed);

			void setStableRegionPoints(vector<XYPair*> *pointList);
			void setPSpacePoints(vector<XYPair*> *pointList);
            void preStoreRules(map<string, set<Rule*>*> *ruleList);
            void addAttribute(Attribute *toAdd);
            vector<Attribute*> *getAttributeList();
            vector<XYPair*> *getStableRegionPoints();
            set<Rule*> *getRulesForStableRegion(XYPair* sr);
			string* getFileName();
            void setFileName(string *fName);

			int lastTransactionID;
	
		private:
			void initializeInstanceFields(string *fileName);
            set<Rule*> *findRules(double support, double confidence);
            set<Rule*> *findRules_wr(double support, double confidence);
			string* getHashForSupportConfidence(double support, double confidence);
			void loadPSpaceIndexFromFile(string *fileName);

			//instance fields.
			string* fileName;
            map<string, set<Rule*>*> *preStoredRules;
			vector<XYPair*> *paramSpacePoints;
			vector<XYPair*> *stableRegionPoints;
            vector<Attribute*> *attributes;
	};
}

#endif


