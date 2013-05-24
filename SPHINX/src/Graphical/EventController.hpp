#ifndef EVENTCONTROLLER_HPP
#define EVENTCONTROLLER_HPP

#include <vector>
#include "graphical/EventListener.hpp"
#include "graphical/IndexUpdateEvent.hpp"
#include "graphical/GUIEventObject.h"

using namespace std;

namespace SPHINXProgram
{
    namespace Graphical
    {
        class EventController
        {
            public:
                EventController(GUIEventObject *g);

                void registerListener(EventListener* el);
                void deregisterListener(EventListener* el);

                void selectStableRegions(Nugget *primarySR, Nugget *secondarySR);
                void applicationSizeChanged(int appWidth, int appHeight);
                void setRuleMode(RuleMode ruleMode);
                void updateIndexInfo(IndexUpdateEvent e);
                void closeIndex();
                void updateGranularity(int g);
                void updateRedundancy(int r);

            private:
                vector<EventListener*> *listeners;
                GUIEventObject *gui;
        };
    }
}
#endif // EVENTCONTROLLER_HPP
