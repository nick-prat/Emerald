#include "../Emerald/entitymanager.hpp"
#include <iostream>
#include <memory>
#include <vector>
#include <unordered_map>
#include <chrono>

using namespace Emerald;

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
        std::cout <<"soemthinn\n";
    }
};

EntityManager entMan;

void createEntities() {
    for(auto a = 0; a < 1000; a++) {
        auto id = entMan.createEntity();
        entMan.createComponent<ComponentA>(id, id);
        entMan.createComponent<ComponentB>(id, id);
        entMan.createComponent<ComponentC>(id, id);
    }
}

int main() {
    createEntities();
    auto start = std::chrono::system_clock::now();
    entMan.mapEntities([](emerald_id id) {
        auto& compa = entMan.getComponent<ComponentA>(id);
        auto& compb = entMan.getComponent<ComponentB>(id);
        auto& compc = entMan.getComponent<ComponentC>(id);
        if(id != compa.getVal()) {
            std::cout << id << " != " << compa.getVal() << '\n';
        }
        if(id != compb.getVal()) {
            std::cout << id << " != " << compb.getVal() << '\n';
        }
        if(id != compc.getVal()) {
            std::cout << id << " != " << compc.getVal() << '\n';
        }
    });
    auto end = std::chrono::system_clock::now();
    auto time = end - start;
    std::cout << "Checks passed in " << std::chrono::duration_cast<std::chrono::microseconds>(time).count() << "us\n";
}
