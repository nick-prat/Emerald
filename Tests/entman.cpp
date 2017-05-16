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

// Using a view seems to be at the very least ~15% faster
int main() {
    createEntities();
    auto view = entMan.getComponentView<ComponentA>();
    std::cout << view.getSize() << '\n';

    auto start = std::chrono::system_clock::now();
    for(int i = 0; i < view.getSize(); i++) {
        fib(view[i].getVal());
    }
    auto end = std::chrono::system_clock::now();
    auto time1 = end - start;

    start = std::chrono::system_clock::now();
    entMan.mapEntities([](emerald_id id) {
        fib(entMan.getComponent<ComponentA>(id).getVal());
    });
    end = std::chrono::system_clock::now();
    auto time2 = end - start;

    std::cout << "Check 1 passed in " << std::chrono::duration_cast<std::chrono::microseconds>(time1).count() << "us\n";
    std::cout << "Check 2 passed in " << std::chrono::duration_cast<std::chrono::microseconds>(time2).count() << "us\n";
    std::cout << "Difference " << (double)(std::chrono::duration_cast<std::chrono::microseconds>(time2).count())
            / std::chrono::duration_cast<std::chrono::microseconds>(time1).count() << '\n';
}
