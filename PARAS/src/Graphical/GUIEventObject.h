#ifndef GUIEVENTOBJECT_H
#define GUIEVENTOBJECT_H

namespace PARASProgram
{
    namespace Graphical
    {
        class GUIEventObject
        {
            public:
                virtual void updateGranularity(int g) = 0;
                virtual void updateRedundancy(bool r) = 0;
        };
    }
}

#endif // GUIEVENTOBJECT_H
