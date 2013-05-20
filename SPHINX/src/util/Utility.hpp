#ifndef UTILITY_H
#define UTILITY_H

#include <string>
#include <vector>
#include "paras/XYPair.hpp"
#include "paras/Rule.hpp"

using namespace std;

namespace PARASProgram
{
	namespace Utility
	{
		string* startTrim(string *s);
		string* endTrim(string *s);
		string* bothTrim(string *s);

		string *arrayToString(vector<string*> *arr, string *buf);

		bool isSubstring(string *strChild, vector<string*> *strParent);

		vector<string*> *numericalOrder(vector<string*> *strToken, bool forItemSets);
		bool strSort(string* a, string* b);
		bool startsWithDigit(string str);
		
		void printRulesVector(vector<Rule*> *lstRules, string msg);
		vector<XYPair*> *distinctPoints(vector<XYPair*> *listOfPoints);

        vector<XYPair*> *buildIntersections(vector<XYPair*> *pspacePoints);
	}
}

#endif
