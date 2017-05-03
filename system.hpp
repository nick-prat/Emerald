#ifndef _SYSTEMS_H
#define _SYSTEMS_H

#include <set>

#include "event.hpp"

namespace Emerald {

    class IBaseSystem {
    public:
        typedef unsigned int system_id;
        virtual void handleEvent(IEvent::event_id eventID, const IBaseEvent& event) = 0;

    protected:
        inline static system_id systemIdCounter = 0;
    };

    template<typename system_t>
    class ISystem : public IBaseSystem {
    public:
        static system_id getSystemID() {
            static system_id systemId = systemIdCounter++;
            return systemId;
        }

        ISystem() = default;
        virtual ~ISystem() {}

        ISystem(const ISystem&) = delete;

        ISystem(ISystem<system_t>&& system)
        : m_entities(std::move(system.m_entities)) {}

        ISystem& operator=(const ISystem&) = delete;
        ISystem& operator=(ISystem&& system) = delete;

        void handleEvent(IBaseEvent::event_id eventID, const IBaseEvent& event) {
            static_cast<system_t*>(this)->handleEvent(eventID);
        }

        void subscribe(unsigned int entityID) {
            m_entities.insert(entityID);
        }

        template<typename entity_manager_t>
        void update(entity_manager_t& entMan) {
            static_cast<system_t*>(this)->update(entMan);
        }

    protected:
        std::set<unsigned int> m_entities;
    };

};

#endif // _SYSTEMS_H
