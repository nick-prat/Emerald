#ifndef _EMERALD_ENTITY_MANAGER_H
#define _EMERALD_ENTITY_MANAGER_H

#include <vector>
#include <memory>
#include <set>
#include <unordered_map>
#include <string>
#include <type_traits>

#include "entity.hpp"
#include "event.hpp"
#include "system.hpp"

namespace Emerald {

    template<typename... comp_ts>
    class EntityManager {
        template<typename T, typename U = void, typename... Ts>
        static constexpr bool _contains() {
            if(std::is_same<U, void>::value) {
                return false;
            } else if(std::is_same<T, U>::value) {
                return true;
            } else {
                return _contains<T, Ts...>();
            }
        }

        template<typename T, typename U = void, typename... Ts>
        static constexpr unsigned int _index(int i = 0) {
            if(std::is_same<T, U>::value) {
                return i;
            } else if (!std::is_same<U, void>::value) {
                return _index<T, Ts...>(++i);
            } else {
                throw std::domain_error("Component type isn't in entity manager");
            }
        }

    public:
        static constexpr std::size_t parameter_size = sizeof...(comp_ts);
        using entity_manager_t = EntityManager<comp_ts...>;
        using entity_t = Entity<entity_manager_t>;

        template<typename comp_t>
        static constexpr bool contains() {
            return _contains<comp_t, comp_ts...>();
        }

        template<typename comp_t>
        static constexpr unsigned int index() {
            static_assert(contains<comp_t>(), "EntityManager::index() called with invalid component type");
            return _index<comp_t, comp_ts...>();
        }

        template<typename comp_t>
        class Component {
            friend class EntityManager;
        public:
            comp_t& getComponent() {
                if(m_enabled) {
                    return m_component;
                } else {
                    throw std::logic_error("component is disabled");
                }
            };

            const comp_t& getComponent() const {
                if(m_enabled) {
                    return m_component;
                } else {
                    throw std::logic_error("component is disabled");
                }
            }

            bool isEnabled() const {
                return m_enabled;
            }

            comp_t* operator->() {
                if(m_enabled) {
                    return &m_component;
                } else {
                    throw std::logic_error("component is disabled");
                }
            }

            unsigned int getEntityID() const {
                return m_entityID;
            }

        private:
            template<typename... args_t>
            Component(int id, args_t&&... args)
            : m_component(std::forward<args_t>(args)...)
            , m_entityID(id)
            , m_enabled(true) {};

        private:
            comp_t m_component;
            unsigned int m_entityID;
            bool m_enabled;
        };

        unsigned int createEntity() {
            if(m_freeLocations.size() > 0) {
                auto loc = m_freeLocations.top();
                m_freeLocations.pop();
                m_entities[loc] = entity_t(loc, this);
                return loc;
            } else {
                auto loc = m_entities.size();
                m_entities.push_back(entity_t(loc, this));
                return loc;
            }
        }

        entity_t& getEntity(unsigned int id) {
            if(id >= m_entities.size() || m_entities[id].getID() != id) {
                throw std::invalid_argument("invalid id");
            } else {
                return m_entities[id];
            }
        }

        const entity_t& getEntity(unsigned int id) const {
            if(id >= m_entities.size() || m_entities[id].getID() != id) {
                throw std::invalid_argument("invalid id");
            } else {
                return m_entities[id];
            }
        }

        std::vector<entity_t>& getEntities() {
            return m_entities;
        }

        const std::vector<entity_t>& getEntities() const {
            return m_entities;
        }

        void removeEntity(unsigned int id) {
            if(id >= m_entities.size() || m_entities[id].getID() == id) {
                m_entities[id] = entity_t();
                m_freeLocations.push(id);
            } else {
                throw std::invalid_argument("invalid id");
            }
        }

        void mapEntities(const std::set<unsigned int>& entities, std::function<void(entity_t&)> func) {
            for(auto id : entities) {
                func(m_entities[id]);
            }
        }

        template<typename comp_t>
        const std::vector<Component<comp_t>>& getComponentList() const {
            static_assert(contains<comp_t>(), "EntityManager::getComponentList() called with invalid type");
            return std::get<std::vector<Component<comp_t>>>(m_componentLists);
        }

        template<typename comp_t, typename... args_t>
        int createComponent(int id, args_t&&... args) {
            static_assert(contains<comp_t>(), "EntityManager::createComponent() called with invalid type");
            auto& list = std::get<std::vector<Component<comp_t>>>(m_componentLists);
            list.push_back(Component<comp_t>(id, std::forward<args_t>(args)...));
            return list.size() - 1;
        }

        template<typename comp_t>
        comp_t& getComponent(unsigned int id) {
            static_assert(contains<comp_t>(), "EntityManager::getComponent() called with invalid type");
            auto& list = std::get<std::vector<Component<comp_t>>>(m_componentLists);
            return list[id].m_component;
        }

        template<typename comp_t>
        const comp_t& getComponent(unsigned int id) const {
            static_assert(contains<comp_t>(), "EntityManager::getComponent() const called with invalid type");
            auto& list = std::get<std::vector<Component<comp_t>>>(m_componentLists);
            return list[id].m_component;
        }

        template<typename comp_t>
        void removeComponent(unsigned int id) {
            static_assert(contains<comp_t>(), "EntityManager::removeComponent() called with invalid type");
            auto& compList = std::get<std::vector<Component<comp_t>>>(m_componentLists);
            compList[id].m_enabled = false;
            compList[id].m_entityID = invalid_id;
            m_freeLocations.push(id);
        }

        template<typename system_t, typename... args_t>
        void registerSystem(args_t&&... args) {
            auto system_id = system_t::getSystemID();
            auto system = m_systems.find(system_id);
            if(system == m_systems.end()) {
                m_systems[system_id] = std::make_unique<system_t>(std::forward<args_t>(args)...);
                m_systemUpdaters[system_id] = [sys = m_systems[system_id].get()](auto& entMan) {
                    static_cast<system_t*>(sys)->update(entMan);
                };
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

        void updateSystems(float timeScale) {
            m_timeScale = timeScale;
            for(auto& updater : m_systemUpdaters) {
                updater.second(*this);
            }
        }

        float getTimeScale() {
            return m_timeScale;
        }

    private:
        float m_timeScale;
        std::stack<unsigned int> m_freeLocations;
        std::vector<entity_t> m_entities;
        std::unordered_map<IBaseSystem::system_id, std::unique_ptr<IBaseSystem>> m_systems;
        std::unordered_map<IBaseSystem::system_id, std::function<void(entity_manager_t&)>> m_systemUpdaters;
        std::tuple<std::vector<Component<comp_ts>>...> m_componentLists;
    };
};

#endif // _EMERALD_ENTITY_MANGER_H
