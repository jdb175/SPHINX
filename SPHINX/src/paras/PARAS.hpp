#ifndef PARAS_HPP
#define PARAS_HPP

#include <vector>
#include <string>
#include "paras/PSpaceIndex.hpp"
#include "paras/PSpaceGenerator.hpp"

using namespace std;

namespace SPHINXProgram
{
	class PARAS
	{
		private:
			//instance fields
			PSpaceGenerator *gene;
			PSpaceIndex *activeIndex;

		public:
            PARAS();
            bool createIndex(string iFileName, string oFileName, string delimiter, double minSup, double minConf, unsigned long sizeThreshold, double precision, bool compress, bool noQuotes);
            bool loadFile(string fileName);
            void loadNewPSpaceIndex(string fileName);
            void loadExistingPSpaceIndex(string fileName);
            void saveActiveIndex(string fileName, bool compressed);
            bool indexOpen();
            void closeIndex();

            set<Rule*> *queryActiveIndex(double support, double confidence, bool redun);
            vector<XYPair*> *getActiveStableRegionPoints();
            set<Rule*> *getRulesForStableRegionPoint(XYPair *sr, bool unique, bool redun);
            vector<Attribute*> *getAttributes();
            string getActiveFileName();
	};
}
#endif
