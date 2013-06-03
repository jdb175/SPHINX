#ifndef INDEXUPDATEEVENT_HPP
#define INDEXUPDATEEVENT_HPP
#include <vector>
#include "paras/Attribute.hpp"
#include "graphical/NPoint"

using namespace SPHINXProgram::Graphical;

namespace SPHINXProgram
{
    namespace Graphical
    {
        class IndexUpdateEvent
        {
            public:
                vector<NPoint*> *stableRegions;
                pair<double, double> supInterval;
                pair<double,double> confInterval;
                pair<int,int> allInterval;
                pair<int,int> uniqueInterval;
                pair<int,int> allInterval_nr;
                pair<int,int> uniqueInterval_nr;
                vector<Attribute*> *attributes;
        };
    }
}


#endif // INDEXUPDATEEVENT_HPP
