#ifndef RULE_H
#define RULE_H

#include <string>
#include <vector>
#include "paras/XYPair.hpp"

using namespace std;

namespace PARASProgram
{
	class Rule
	{
		private:
			vector<string*> *strCombination, *strRemaining;
			double _support, _confidence;

			XYPair *_dominatePoint_simple;
			XYPair *_dominatePoint_strict;

		public:
			Rule(vector<string*> *strCombination, vector<string*> *strRemaining, double Support, double Confidence, vector<XYPair*> *DominatePoints);
            ~Rule();
            vector<string*> *getX();

			vector<string*> *getY();

			double getSupport();
			double getConfidence();

			XYPair *getDominantPointSimple();
			void setDominantPointSimple(XYPair *value);

			XYPair *getDominantPointStrict();
			void setDominantPointStrict(XYPair *value);
		
			string* ToListString();
			string* XToListString();
			string* YToListString();
            Json::Value jsonify();

            vector<string*> *displayAnte;
            vector<string*> *displayCons;

			bool operator<(Rule *other);
			bool operator>(Rule *other);
			bool operator==(Rule *other);
	};
}

#endif
