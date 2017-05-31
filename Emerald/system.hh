#ifndef _SYSTEMS_H
#define _SYSTEMS_H

#include <set>

#include "event.hpp"
#include "Util/types.hpp"

namespace Emerald {

    class EntityManager;

    class IBaseSystem {
    public:
        virtual void update(EntityManager&) = 0;

    protected:
        inline static emerald_id systemIdCounter = 0;
    };

    template<typename system_t>
    class ISystem : public IBaseSystem {
    public:
        static emerald_id getSystemID() {
            static emerald_id systemId = systemIdCounter++;
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
            static_cast<system_t*>(this)->handleEvent(eventID, event);
        }

        void subscribe(emerald_id entityID) {
            m_entities.insert(entityID);
        }

        void update(EntityManager& entMan) {
            static_cast<system_t*>(this)->update(entMan);
        }

    protected:
        std::set<emerald_id> m_entities;
    };

    template<typename system_t>
    inline emerald_id getSystemID() {
        return system_t::getSystemID();
    }

};

#endif // _SYSTEMS_H
