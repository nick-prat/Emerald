#include <iostream>
#include "../Emerald/entitymanager.hpp"

using namespace Emerald;

class ComponentA {
public:
    ComponentA(emerald_id id) : m_val(id) {};
    emerald_id getVal() const {
        return m_val;
    }

    ~ComponentA() {
        std::cout << "Deleting component " << m_val << '\n';
    }

private:
    emerald_id m_val;
};

class System : public ISystem<System> {
public:
    void update(EntityManager& entMan) {
        auto view = entMan.getComponentView<ComponentA>();
        for(emerald_id i = 0; i < view.getSize(); i++) {
            try {
                std::cout << view[i].getVal() << '\n';
            } catch(bad_id err) {
                std::cout << err.what() << '\n';
            }
        }
    }
};

int main() {
    EntityManager entMan;
    entMan.registerSystem<System>();
    for(auto i = 0; i < 100; i++) {
        auto id = entMan.createEntity();
        std::cout << id << '\n';
        entMan.createComponent<ComponentA>(id, id);
    }
    entMan.removeEntity(20);
    entMan.updateSystems(1.0f);
}
