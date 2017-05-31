#ifndef _EMERALD_ENTITY_MANAGER_H
#define _EMERALD_ENTITY_MANAGER_H

#include <vector>
#include <memory>
#include <array>
#include <unordered_map>
#include <iostream>
#include "Util/types.hh"
#include "Util/exceptions.hh"
#include "component.hh"
#include "system.hh"

namespace Emerald {

    class EntityManager {
    private:
        template<typename t> struct identity { typedef t type; };

    public:
        EntityManager() : m_entityCount(0) {}

        emerald_id createEntity() {
            m_entities[m_entityCount] = std::vector<emerald_long>();
            return m_entityCount++;
        }

        void removeEntity(const emerald_id id) {
            if(auto iter = m_entities.find(id); iter != m_entities.end()) {
                for(auto id : iter->second) {
                    emerald_id type = (id >> 16);
                    emerald_id loc = id & comp_id_mask;
                    m_components[type]->deleteComponent(loc);
                }
                m_entities.erase(iter);
            }
        }

        template<typename... comp_ts>
        void mapEntities(std::function<void(const emerald_id)> func) {
            for(auto& [id, list] : m_entities) {
                if(entityHasComponents<comp_ts...>(id)) {
                    func(id);
                }
            }
        }

        template<typename... comp_ts>
        bool entityHasComponents(const emerald_id entID) const {
            return ((entityHasComponent<comp_ts>(entID) != invalid_id) && ...);
        }

        template<typename comp_t>
        emerald_id entityHasComponent(const emerald_id entID) const {
            if(auto iter = m_entities.find(entID); iter != m_entities.end()) {
                auto compID = getComponentID<comp_t>();
                for(auto comptag : iter->second) {
                    if((comptag & comp_type_mask) >> 16 == compID) {
                        return comptag & comp_id_mask;
                    }
                }
            }
            return invalid_id;
        }

        template<typename comp_t>
        PoolView<comp_t> getComponentView() {
            if(auto iter = m_components.find(getComponentID<comp_t>()); iter != m_components.end()) {
                return static_cast<ComponentPool<comp_t>*>(iter->second.get())->getComponentView();
            } else {
                throw BadType("getComponentView invalid component type");
            }
        }

        template<typename comp_t, typename... args_t>
        emerald_id createComponent(const emerald_id id, args_t&&... args) {
            auto compID = getComponentID<comp_t>();
            if(auto cid = entityHasComponent<comp_t>(id); cid != invalid_id) {
                return cid & comp_id_mask;
            }
            if(m_components.find(compID) == m_components.end()) {
                m_components[compID] = std::make_unique<ComponentPool<comp_t>>();
            }
            auto cid = static_cast<ComponentPool<comp_t>*>(m_components[compID].get())->createComponent(id, std::forward<args_t>(args)...);
            m_entities[id].push_back((compID << 16) | cid);
            return cid;
        }

        template<typename comp_t>
        comp_t& getComponent(const emerald_id id) {
            auto compID = getComponentID<comp_t>();
            if(auto loc = entityHasComponent<comp_t>(id); loc != invalid_id) {
                return static_cast<ComponentPool<comp_t>*>(m_components[compID].get())->getComponent(loc);
            } else {
                throw BadType("getComponent invalid component type");
            }
        }

        template<typename comp_t>
        const comp_t& getComponent(const emerald_id id) const {
            auto compID = getComponentID<comp_t>();
            auto loc = entityHasComponent<comp_t>(id);
            if(loc != invalid_id) {
                if(auto iter = m_components.find(compID); iter != m_components.end()) {
                    auto& [id, uptr] = *iter;
                    return static_cast<const ComponentPool<comp_t>*>(uptr.get())->getComponent(loc);
                } else {
                    throw BadType("getComponent const invalid component type");
                }
            } else {
                throw BadType("getComponent const Entity doesn't have component");
            }
        }

        template<typename comp_t>
        void removeComponent(const emerald_id id) {
            auto iter = m_components.find(getComponentID<comp_t>());
            if(iter != m_components.end()) {
                iter->second->deleteComponent(id);
            }
        }

        template<typename... comp_ts>
        void mapComponents(typename identity<std::function<void(comp_ts&...)>>::type func) {
            for(auto [id, cv] : m_entities) {
                if(entityHasComponents<comp_ts...>(id)) {
                    func(static_cast<ComponentPool<comp_ts>*>(m_components[getComponentID<comp_ts>()].get())->getComponent(id)...);
                }
            }
        }

        template<typename system_t, typename... args_t>
        void registerSystem(args_t&&... args) {
            const auto system_id = system_t::getSystemID();
            if(auto iter = m_systems.find(system_id); iter == m_systems.end()) {
                m_systems[system_id] = std::make_unique<system_t>(std::forward<args_t>(args)...);
            } else {
                BadType("System of type already exists");
            }
        }

        template<typename system_t>
        system_t& getSystem() {
            auto system = m_systems.find(system_t::getSystemID());
            if(system != m_systems.end()) {
                return *(static_cast<system_t*>(system->second.get()));
            } else {
                throw BadSystem("System not found");
            }
        }

        template<typename system_t>
        const system_t& getSystem() const {
            auto system = m_systems.find(system_t::getSystemID());
            if(system != m_systems.end()) {
                return *(system->second.get());
            } else {
                throw BadSystem("System not found");
            }
        }

        void updateSystems(const float timeScale) {
            m_timeScale = timeScale;
            for(auto& iter : m_systems) {
                iter.second->update(*this);
            }
        }

    private:
        float m_timeScale;
        std::size_t m_entityCount;
        std::unordered_map<emerald_id, std::vector<emerald_long>> m_entities;
        std::unordered_map<emerald_id, std::unique_ptr<IBaseSystem>> m_systems;
        std::unordered_map<emerald_id, std::unique_ptr<IBaseComponentPool>> m_components;
    };

};

#endif // _EMERALD_ENTITY_MANGER_H
