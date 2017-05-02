#ifndef _SYSTEMS_H
#define _SYSTEMS_H

#include <iostream>
#include <set>

namespace Emerald {

    template<typename system_t>
    class ISystem {
    public:
        using system_id = unsigned int;

        static system_id getSystemID() {
            static system_id systemId = systemIDCounter++;
            return systemId;
        }

        ISystem() = default;
        virtual ~ISystem() {}

        ISystem(const ISystem&) = delete;

        ISystem(ISystem<system_t>&& system)
        : m_entities(std::move(system.m_entities)) {}

        ISystem& operator=(const ISystem&) = delete;
        ISystem& operator=(ISystem&& system) = delete;

        void subscribe(unsigned int entityID) {
            m_entities.insert(entityID);
        }

        template<typename entity_manager_t>
        void update(entity_manager_t& entMan) {
            static_cast<system_t*>(this)->update(entMan);
        }

    protected:
        inline static system_id systemId;
        std::set<unsigned int> m_entities;

    private:
        inline static system_id systemIDCounter = 0;
    };

};

#endif // _SYSTEMS_H
