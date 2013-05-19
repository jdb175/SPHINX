#ifndef PSPACEGENERATOR_H
#define PSPACEGENERATOR_H

#include <string>
#include <iostream>
#include <vector>
#include <set>
#include "PSpaceIndex.hpp"
#include "Rule.hpp"

using namespace std;

namespace PARASProgram
{
	class PSpaceGenerator
	{
		public:
			void generatePSpaceIndex(PSpaceIndex *targetIndex);
			
			PSpaceGenerator() 
			{
				initializePSpaceGenerator();
			}

		private:
			void initializePSpaceGenerator();

			void loadFile(string *fileName);

            void readAttributes(ifstream *din);

			void readRulesFromFile(ifstream *din);
			void buildRuleFromStr(string str);
			void readItemSetsFromFile(string *str);

			void findStableRegions();
			vector<Rule*> *buildRedund();
            map<string, set<Rule*>*> *storeRules();
	
			PSpaceIndex *activeIndex;
            double totalOccurences;
            set<string> *domain;
            vector<Rule*> *strongRulesList;
			map<string, double> *frequentItemsDictionary;
            bool mapped;
	};
}

#endif
