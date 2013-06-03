#ifndef EVENTLISTENER_H
#define EVENTLISTENER_H
#include "graphical/NPoint"
#include "graphical/IndexUpdateEvent.hpp"

namespace SPHINXProgram
{
    namespace Graphical
    {
        class EventListener
        {
            public:
                virtual void selectStableRegions(NPoint *primarySR, NPoint *secondarySR) = 0;
                virtual void applicationSizeChanged(int width, int height) = 0;
                virtual void setRuleMode(RuleMode ruleMode) = 0;
                virtual void updateIndexInfo(IndexUpdateEvent e) = 0;
                virtual void updateRedundancy(bool include) = 0;
                virtual void closeIndex() = 0;
        };
    }
}

#endif // EVENTLISTENER_H
