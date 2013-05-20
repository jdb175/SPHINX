#include "graphical/EventController.hpp"
#include "graphical/StableRegion.hpp"
#include "graphical/EventListener.hpp"

using namespace PARASProgram::Graphical;


EventController::EventController(GUIEventObject *g)
{
    listeners = new vector<EventListener*>();
    gui = g;
}

void EventController::registerListener(EventListener* el)
{
    listeners->push_back(el);
}

void EventController::deregisterListener(EventListener *el)
{
    for(size_t i = 0; i < listeners->size(); i++)
    {
        if(listeners->at(i) == el)
        {
            listeners->erase(listeners->begin() + i);
            break;
        }
    }
}

void EventController::selectStableRegions(StableRegion *primarySR, StableRegion *secondarySR)
{
    for(size_t i = 0; i < listeners->size(); i++)
    {
        listeners->at(i)->selectStableRegions(primarySR, secondarySR);
    }
}

void EventController::applicationSizeChanged(int appWidth, int appHeight)
{
    for(size_t i = 0; i < listeners->size(); i++)
    {
        listeners->at(i)->applicationSizeChanged(appWidth, appHeight);
    }
}

void EventController::setRuleMode(RuleMode ruleMode)
{
    for(size_t i = 0; i < listeners->size(); i++)
    {
        listeners->at(i)->setRuleMode(ruleMode);
    }
}

void EventController::updateIndexInfo(IndexUpdateEvent e)
{
    for(size_t i = 0; i < listeners->size(); i++)
    {
        listeners->at(i)->updateIndexInfo(e);
    }
}

void EventController::closeIndex()
{
    for(size_t i = 0; i < listeners->size(); i++)
    {
        listeners->at(i)->closeIndex();
    }
}

void EventController::updateGranularity(int g){
    gui->updateGranularity(g);
}

void EventController::updateRedundancy(int r) {
    for(size_t i = 0; i < listeners->size(); i++)
    {
        listeners->at(i)->updateRedundancy(r);
    }

    gui->updateRedundancy(r);
}

