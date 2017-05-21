#ifndef _EMERALD_COMPONENT_H
#define _EMERALD_COMPONENT_H

#include <functional>
#include <stack>
#include <cstdlib>
#include <cstring>
#include "Util/types.hpp"
#include "Util/exceptions.hpp"

namespace Emerald {

    class IBaseComponent {
    public:
        IBaseComponent(emerald_id entID)
        : m_entityID(entID)
        , m_enabled(true) {}

        IBaseComponent(IBaseComponent&& comp) {
            m_entityID = comp.m_entityID;
            m_enabled = comp.m_enabled;
        }

        virtual ~IBaseComponent() {
            m_entityID = invalid_id;
            m_enabled = false;
        };

        bool isEnabled() const {
            return m_enabled;
        }

        emerald_id getEntityID() const {
            return m_entityID;
        }

    protected:
        inline static emerald_id componentIDCounter = 0;
        emerald_id m_entityID;
        bool m_enabled;
    };

    template<typename comp_t>
    class Component : public IBaseComponent {
    public:
        static emerald_id getComponentID() {
            static emerald_id componentID = componentIDCounter++;
            return componentID;
        }

        template<typename... args_t>
        Component(int id, args_t&&... args)
        : IBaseComponent(id)
        , m_component(std::forward<args_t>(args)...) {}

        Component(Component<comp_t>&& comp)
        : IBaseComponent(std::move(comp))
        , m_component(std::move(comp.m_component)) {}

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

        comp_t& operator*() {
            return getComponent();
        }

        const comp_t& operator*() const {
            return getComponent();
        }

        comp_t* operator->() {
            return &(getComponent());
        }

        const comp_t* operator->() const {
            return &(getComponent());
        }

    protected:
        comp_t m_component;
    };

    template<typename comp_t>
    class PoolViewIter {
    public:
        PoolViewIter(Component<comp_t>* view, emerald_id loc, emerald_id end)
        : m_view(view)
        , m_loc(loc)
        , m_end(end) {
            while(m_loc != m_end && m_view[m_loc].isEnabled() == false) {
                m_loc++;
            }
        }

        bool operator==(const PoolViewIter<comp_t>& other) const {
            return m_loc == other.m_loc;
        }

        bool operator!=(const PoolViewIter<comp_t>& other) const {
            return m_loc != other.m_loc;
        }

        PoolViewIter& operator++() {
            do {
                m_loc++;
            } while(m_loc != m_end && m_view[m_loc].isEnabled() == false);
            return *this;
        }

        PoolViewIter operator++(int) {
            PoolViewIter<comp_t> tmp(*this);
            operator++();
            return tmp;
        }

        comp_t* operator->() {
            return &(m_view[m_loc].getComponent());
        }

        const comp_t* operator->() const {
            return &(m_view[m_loc].getComponent());
        }

        comp_t& operator*() {
            return m_view[m_loc].getComponent();
        }

        const comp_t& operator*() const {
            return m_view[m_loc].getComponent();
        }

    private:
        Component<comp_t>* const m_view;
        emerald_id m_loc;
        const emerald_id m_end;
    };

    template<typename comp_t>
    class ConstPoolViewIter {
    public:
        ConstPoolViewIter(Component<comp_t>* view, emerald_id loc, emerald_id end)
        : m_view(view)
        , m_loc(loc)
        , m_end(end) {
            while(m_loc != m_end && m_view[m_loc].isEnabled() == false) {
                m_loc++;
            }
        }

        bool operator==(const ConstPoolViewIter<comp_t>& other) const {
            return m_loc == other.m_loc;
        }

        bool operator!=(const ConstPoolViewIter<comp_t>& other) const {
            return m_loc != other.m_loc;
        }

        ConstPoolViewIter& operator++() {
            do {
                m_loc++;
            } while(m_loc != m_end && m_view[m_loc].isEnabled() == false);
            return *this;
        }

        ConstPoolViewIter operator++(int) {
            PoolViewIter<comp_t> tmp(*this);
            operator++();
            return tmp;
        }

        const comp_t* operator->() const {
            return &(m_view[m_loc].getComponent());
        }

        const comp_t& operator*() const {
            return m_view[m_loc].getComponent();
        }

    private:
        const Component<comp_t>* const m_view;
        emerald_id m_loc;
        const emerald_id m_end;
    };

    template<typename comp_t>
    class PoolView {
    public:
        PoolView(Component<comp_t>* view, std::size_t size) noexcept
        : m_view(view)
        , m_size(size) {}

        PoolViewIter<comp_t> begin() {
            return PoolViewIter<comp_t>(m_view, 0, m_size);
        }

        PoolViewIter<comp_t> end() {
            return PoolViewIter<comp_t>(m_view, m_size, m_size);
        }

        ConstPoolViewIter<comp_t> begin() const {
            return ConstPoolViewIter<comp_t>(m_view, 0, m_size);
        }

        ConstPoolViewIter<comp_t> end() const {
            return ConstPoolViewIter<comp_t>(m_view, m_size, m_size);
        }

        std::size_t getSize() const {
            return m_size;
        }

        bool contains(emerald_id id) const {
            return id < m_size || !m_view[id].isEnabled();
        }

        comp_t& operator[](emerald_id id) {
            if(m_view[id].isEnabled() && id < m_size) {
                return m_view[id].getComponent();
            } else {
                throw bad_id("PoolView::operator[] invalid id");
            }
        }

        const comp_t& operator[](emerald_id id) const {
            if(m_view[id].isEnabled() && id < m_size) {
                return m_view[id].getComponent();
            } else {
                throw bad_id("PoolView::operator[] invalid id");
            }
        }

        void map(std::function<void(comp_t&)> func) {
            for(std::size_t i = 0; i < m_size; i++) {
                if(m_view[i].isEnabled()) {
                    func(m_view[i].getComponent());
                }
            }
        }

        void map(std::function<void(const comp_t&)> func) const {
            for(std::size_t i = 0; i < m_size; i++) {
                if(m_view[i].isEnabled()) {
                    func(m_view[i].getComponent());
                }
            }
        }

    private:
        Component<comp_t>* m_view;
        std::size_t m_size;
    };

    class IBaseComponentPool {
    public:
        virtual ~IBaseComponentPool() = default;
        virtual void deleteComponent(const emerald_id location) = 0;
    };

    template<typename comp_t>
    class ComponentPool : public IBaseComponentPool {
    public:
        ComponentPool(const std::size_t amount = 10)
        : m_poolBasePtr(reinterpret_cast<Component<comp_t>*>(malloc(sizeof(Component<comp_t>) * amount)))
        , m_poolTop(0)
        , m_poolSize(amount) {
            static_assert(std::is_nothrow_move_constructible<comp_t>::value, "Component must be no-throw move construcible");
        };

        ~ComponentPool() {
            if(m_poolBasePtr != nullptr) {
                for(Component<comp_t>* itr = m_poolBasePtr; itr < m_poolBasePtr + m_poolTop; itr++) {
                    if(itr->isEnabled()) {
                        itr->~IBaseComponent();
                    }
                }
                free(m_poolBasePtr);
            }
        }

        template<typename... comp_args_t>
        emerald_id createComponent(const emerald_id entID, comp_args_t... args) {
            emerald_id location = 0;
            if(m_freeLocations.size() > 0) {
                location = m_freeLocations.top();
                m_freeLocations.pop();
            } else if(m_poolTop >= m_poolSize) {
                Component<comp_t>* newPtr = reinterpret_cast<Component<comp_t>*>(malloc(sizeof(Component<comp_t>) * m_poolSize * 2));
                for(auto i = 0; i < m_poolSize; i++) {
                    new(&newPtr[i]) Component<comp_t>(std::move(m_poolBasePtr[i]));
                    m_poolBasePtr[i].~Component<comp_t>();
                }
                free(m_poolBasePtr);
                m_poolBasePtr = newPtr;
                m_poolSize *= 2;
                return createComponent(entID, std::forward<comp_args_t>(args)...);
            } else {
                location = m_poolTop;
                m_poolTop++;
            }
            new(m_poolBasePtr + location) Component<comp_t>(entID, std::forward<comp_args_t>(args)...);
            return location;
        }

        void deleteComponent(const emerald_id location) {
            if((m_poolBasePtr + location)->isEnabled()) {
                (m_poolBasePtr + location)->~Component<comp_t>();
                m_freeLocations.push(location);
            }
        }

        PoolView<comp_t> getComponentView() {
            return PoolView<comp_t>(m_poolBasePtr, m_poolTop);
        };

        const PoolView<comp_t> getComponentView() const {
            return PoolView<comp_t>(m_poolBasePtr, m_poolTop);
        }

        comp_t& getComponent(emerald_id id) {
            if(id < m_poolSize && m_poolBasePtr[id].isEnabled()) {
                return *(m_poolBasePtr[id]);
            } else {
                throw std::runtime_error("getComponent() invalid id");
            }
        }

        const comp_t& getComponent(emerald_id id) const {
            if(id < m_poolSize && m_poolBasePtr[id].isEnabled()) {
                return *(m_poolBasePtr[id]);
            } else {
                throw std::runtime_error("getComponent() invalid id");
            }
        }

    private:
        Component<comp_t>* m_poolBasePtr;
        emerald_id m_poolTop;
        std::stack<emerald_id> m_freeLocations;
        std::size_t m_poolSize;
    };
};

#endif // _EMERALD_COMPONENT_H
