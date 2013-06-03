#ifndef PSPACEINDEX_H
#define PSPACEINDEX_H

#include <string>
#include <vector>
#include <map>
#include <set>
#include "paras/XYPair.hpp"
#include "graphical/NPoint.hpp"
#include "paras/Attribute.hpp"

using namespace std;

namespace SPHINXProgram
{
	class PSpaceIndex
	{
		public:
            PSpaceIndex(string *fileName, bool alreadyGenerated);
            ~PSpaceIndex();
			//public functions.
            set<Nugget*> *getNuggets();
            set<Nugget*> *getNuggets(string hypothesisName);
            void savePSpaceIndexToFile(string file, bool compressed);
			string* getFileName();
            void setFileName(string *fName);
	
		private:
			void initializeInstanceFields(string *fileName);
            void *addHypothesisObject(string hypothesisName, set<void*>*existingHypotheses);
			void loadPSpaceIndexFromFile(string *fileName);
            void generatePSpaceIndexFromFile(string *fileName);

			//instance fields.
			string* fileName;
            set<Nugget*> allNuggets;
            map<string, set<Nugget*>*> hypothesisNuggets;
	};
}

#endif


