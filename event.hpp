#ifndef _EMERALD_EVENT_H
#define _EMERALD_EVENT_H

namespace Emerald {

    class IEvent {
    public:
        using event_id = unsigned int;

        static event_id getEventID() {
            static event_id eventID = eventIDCounter++;
            return eventID;
        };

    private:
        inline static event_id eventIDCounter = 0;
    };

};

#endif // _EMERALD_EVENT_H
