#ifndef _EMERALD_ENTITY_MANAGER_H
#define _EMERALD_ENTITY_MANAGER_H

#include <vector>
#include <memory>
#include <unordered_map>

#include "Util/types.hpp"
#include "Util/exceptions.hpp"
#include "component.hpp"
#include "event.hpp"
#include "system.hpp"

namespace Emerald {

    class EntityManager {
    public:
        unsigned int createEntity() {
            m_entities[m_entityCount] = std::vector<emerald_long>();
            return m_entityCount++;
        }

        void removeEntity(unsigned int id) {
            if(auto iter = m_entities.find(id); iter != m_entities.end()) {
                for(auto id : iter->second) {
                    emerald_id type = (id >> 16);
                    emerald_id loc = id & comp_id_mask;
                    m_components[type]->deleteComponent(loc);
                }
                m_entities.erase(iter);
            }
        }

        void mapEntities(std::function<void(emerald_id)> func) {
            for(auto& [id, list] : m_entities) {
                func(id);
            }
        }

        template<typename comp_t>
        const std::vector<Component<comp_t>>& getComponentList() const {
            auto iter = m_components.find(comp_t::getComponentID());
            if(iter != m_components.end()) {

            } else {

            }
        }

        template<typename comp_t, typename... args_t>
        emerald_id createComponent(emerald_id id, args_t&&... args) {
            auto compID = Component<comp_t>::getComponentID();
            if(auto cid = entityHasComponent(id, compID); cid != invalid_id) {
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
        comp_t& getComponent(emerald_id id) {
            auto compID = Component<comp_t>::getComponentID();
            if(auto loc = entityHasComponent(id, compID); loc != invalid_id) {
                return static_cast<ComponentPool<comp_t>*>(m_components[compID].get())->getComponent(loc);
            } else {
                throw bad_component("getComponent invalid component type");
            }
        }

        template<typename comp_t>
        const comp_t& getComponent(emerald_id id) const {
            auto compID = Component<comp_t>::getComponentID();
            auto loc = entityHasComponent(id, compID);
            if(loc != invalid_id) {
                if(auto iter = m_components.find(compID); iter != m_components.end()) {
                    auto& [id, uptr] = *iter;
                    return static_cast<const ComponentPool<comp_t>*>(uptr.get())->getComponent(loc);
                } else {
                    throw bad_component("getComponent const invalid component type");
                }
            } else {
                throw bad_component("getComponent const Entity doesn't have component");
            }
        }

        template<typename comp_t>
        void removeComponent(unsigned int id) {
            auto iter = m_components.find(comp_t::getComponentID());
            if(iter != m_components.end()) {
                iter->second->deleteComponent(id);
            }
        }

        template<typename system_t, typename... args_t>
        void registerSystem(args_t&&... args) {
            const auto system_id = system_t::getSystemID();
            if(auto iter = m_systems.find(system_id); iter == m_systems.end()) {
                m_systems[system_id] = std::make_unique<system_t>(std::forward<args_t>(args)...);
            } else {
                // TODO Throw proper error
            }
        }

        template<typename system_t>
        system_t& getSystem() {
            auto system = m_systems.find(system_t::getSystemID());
            if(system != m_systems.end()) {
                return *(static_cast<system_t*>(system->second.get()));
            } else {
                throw std::logic_error("system not found");
            }
        }

        template<typename system_t>
        const system_t& getSystem() const {
            auto system = m_systems.find(system_t::getSystemID());
            if(system != m_systems.end()) {
                return *(system->second.get());
            } else {
                throw std::logic_error("system not found");
            }
        }

        void updateSystems(const float timeScale) {
            m_timeScale = timeScale;
            for(auto& iter : m_systems) {
                iter.second->update(*this);
            }
        }

    private:
        emerald_id entityHasComponent(const emerald_id entID, const emerald_id compID) const {
            if(auto iter = m_entities.find(entID); iter != m_entities.end()) {
                for(auto comptag : iter->second) {
                    if((comptag & comp_type_mask) >> 16 == compID) {
                        return comptag & comp_id_mask;
                    }
                }
            }
            return invalid_id;
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
