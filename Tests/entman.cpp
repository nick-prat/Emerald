#include "../Emerald/entitymanager.hpp"
#include <iostream>
#include <memory>
#include <vector>
#include <unordered_map>
#include <chrono>

using namespace Emerald;
using namespace std;

auto fib(int n) {
    auto v1 = -1;
    auto v2 = 1;
    auto total = 0;
    for(int i = 0; i <= n; i++) {
        total = v2 + v1;
        v1 = v2;
        v2 = total;
    }
    return v2;
}

class ComponentA {
public:
    ComponentA(int val) : m_val(val) {};
    ComponentA(const ComponentA&) = delete;
    ComponentA(ComponentA&&) noexcept = default;
    int getVal() {
        return m_val;
    }
    void print() const {
        std::cout << "CA: " << m_val << '\n';
    }
private:
    int m_val;
};

class ComponentB {
public:
    ComponentB(int val) : m_val(val) {};
    ComponentB(const ComponentA&) = delete;
    ComponentB(ComponentB&&) noexcept = default;
    int getVal() {
        return m_val;
    }
    void print() const {
        std::cout << "CB: " <<  m_val << '\n';
    }
private:
    int m_val;
};

class ComponentC {
public:
    ComponentC(int val) : m_val(val) {};
    ComponentC(const ComponentA&) = delete;
    ComponentC(ComponentC&&) noexcept = default;
    int getVal() {
        return m_val;
    }
    void print() const {
        std::cout << "CC: " << m_val << '\n';
    }
private:
    int m_val;
};

class sys : public ISystem<sys> {
public:
    void update(EntityManager& entMan) {
        auto start = chrono::system_clock::now();
        auto aview = entMan.getComponentView<ComponentA>();
        auto cview = entMan.getComponentView<ComponentC>();
        entMan.mapEntities<ComponentA, ComponentC>([&aview, &cview, &entMan](emerald_id ent) {
            auto& compa = aview[entMan.entityHasComponent<ComponentA>(ent)];
            auto& compc = cview[entMan.entityHasComponent<ComponentC>(ent)];
            if(compa.getVal() != compc.getVal()) {
                std::cout << "error\n";
            }
        });
        std::cout << "system1 in " << chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now() - start).count() << '\n';
        start = chrono::system_clock::now();
        entMan.mapComponents<ComponentA, ComponentC>([](auto& ca, auto& cc) {
            if(ca.getVal() != cc.getVal()) {
                std::cout << "error\n";
            }
        });
        std::cout << "system2 in " << chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now() - start).count() << '\n';
    }
};

EntityManager entMan;

void createEntities() {
    auto& s = entMan.getSystem<sys>();
    for(auto a = 0; a < 1000; a++) {
        auto id = entMan.createEntity();
        entMan.createComponent<ComponentA>(id, id);
        entMan.createComponent<ComponentB>(id, id);
        entMan.createComponent<ComponentC>(id, id);
        s.subscribe(id);
    }
}

// Using a view seems to be at the very least ~15% faster
int main() {
    entMan.registerSystem<sys>();
    createEntities();

    entMan.updateSystems(1.0f);

    if(entMan.entityHasComponents<ComponentA, ComponentB, ComponentC>(0)) {
        std::cout << "Entity has components!\n";
    } else {
        std::cout << "Entity does not have components\n";
    }

    int total = 0;
    auto mstart = std::chrono::system_clock::now();
    entMan.mapComponents<ComponentA, ComponentC>([](auto& ca, auto& cc) {
        if(ca.getVal() != cc.getVal()) {
            std::cout << "Mapping failure\n";
        }
    });
    std::cout << "Map timing " << std::chrono::duration_cast<chrono::microseconds>(std::chrono::system_clock::now() - mstart).count() << '\n';

    total = 0;
    auto cstart = std::chrono::system_clock::now();
    for(auto& comp : entMan.getComponentView<ComponentA>()) {
        fib(comp.getVal());
    }
    for(auto& comp : entMan.getComponentView<ComponentC>()) {
        fib(comp.getVal());
    }
    std::cout << "Iter timing " << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - cstart).count() << '\n';

    auto start = std::chrono::system_clock::now();
    auto viewa = entMan.getComponentView<ComponentA>();
    for(int i = 0; i < viewa.getSize(); i++) {
        fib(viewa[i].getVal());
    }
    auto viewb = entMan.getComponentView<ComponentA>();
    for(int i = 0; i < viewb.getSize(); i++) {
        fib(viewb[i].getVal());
    }
    auto viewc = entMan.getComponentView<ComponentA>();
    for(int i = 0; i < viewc.getSize(); i++) {
        fib(viewc[i].getVal());
    }
    auto end = std::chrono::system_clock::now();
    auto time1 = end - start;

    std::cout << "begging map\n";

    start = std::chrono::system_clock::now();
    entMan.mapEntities([](emerald_id id) {
        fib(entMan.getComponent<ComponentA>(id).getVal());
        fib(entMan.getComponent<ComponentB>(id).getVal());
        fib(entMan.getComponent<ComponentC>(id).getVal());
    });
    end = std::chrono::system_clock::now();
    auto time2 = end - start;

    std::cout << "Check 1 passed in " << std::chrono::duration_cast<std::chrono::microseconds>(time1).count() << "us\n";
    std::cout << "Check 2 passed in " << std::chrono::duration_cast<std::chrono::microseconds>(time2).count() << "us\n";
    std::cout << "Difference " << (double)(std::chrono::duration_cast<std::chrono::microseconds>(time2).count())
            / std::chrono::duration_cast<std::chrono::microseconds>(time1).count() << '\n';
}
