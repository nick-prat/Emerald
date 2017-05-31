#ifndef _EMERALD_EVENT_H
#define _EMERALD_EVENT_H

namespace Emerald {

    class IBaseEvent {
    public:
        typedef unsigned int event_id;

    protected:
        inline static event_id eventIDCounter = 0;

    };

    template<typename event_t>
    class IEvent : public IBaseEvent {
    public:
        static event_id getEventID() {
            static event_id eventID = eventIDCounter++;
            return eventID;
        };
    };

};

#endif // _EMERALD_EVENT_H
