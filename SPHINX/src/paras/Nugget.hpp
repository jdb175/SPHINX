﻿#ifndef RULE_H
#define RULE_H

#include <string>
#include <vector>
#include "paras/XYPair.hpp"

using namespace std;

namespace SPHINXProgram
{
    class Nugget
	{
		private:
            double _belief, _plausibility;
            string hypothesis;
            string evidenceType;

		public:
            Nugget(double belief, double plausiblity, string hypothesis, string evidenceType);
            ~Nugget();

            double getBelief();
            double getPlausibility();

            Json::Value jsonify();

            vector<string*> *displayAnte;
            vector<string*> *displayCons;

            bool operator<(Nugget *other);
            bool operator>(Nugget *other);
            bool operator==(Nugget *other);
	};
}

#endif
