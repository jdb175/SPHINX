#ifndef EVENTLISTENER_H
#define EVENTLISTENER_H
#include "graphical/Nugget.hpp"
#include "graphical/IndexUpdateEvent.hpp"

namespace PARASProgram
{
    namespace Graphical
    {
        class EventListener
        {
            public:
                virtual void selectStableRegions(Nugget *primarySR, Nugget *secondarySR) = 0;
                virtual void applicationSizeChanged(int width, int height) = 0;
                virtual void setRuleMode(RuleMode ruleMode) = 0;
                virtual void updateIndexInfo(IndexUpdateEvent e) = 0;
                virtual void updateRedundancy(bool include) = 0;
                virtual void closeIndex() = 0;
        };
    }
}

#endif // EVENTLISTENER_H
