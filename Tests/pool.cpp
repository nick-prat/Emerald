#include "../Emerald/component.hpp"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <unordered_map>

using namespace Emerald;

class AComponent {
public:
    AComponent() noexcept
    : m_val(rand() % 100) {
        std::cout << "constructing a\n";
    }

    AComponent(AComponent&& comp) noexcept {
        m_val = comp.m_val;
    }

    ~AComponent() noexcept {
        std::cout << "destructing a\n";
    }

    static Emerald::emerald_id getComponentID() {
        return 0;
    }

    void print() {
        std::cout << m_val << " nonconst\n";
    }

    void print() const {
        std::cout << m_val << " const\n";
    }

private:
    int m_val;
};

class BComponent {
public:
    BComponent() {
        std::cout << "constructing b\n";
    }

    ~BComponent() noexcept {
        std::cout << "destructing b " << this << " \n";
    }

};

int main() {
    std::unordered_map<int, int> map;
    
    if(auto iter = map.find(10); iter != map.end()) {
        auto& [key, val] = iter.operator*();
        std::cout << val << '\n';
        val = 100;
    }

    srand(time(0));
    ComponentPool<AComponent> poola(10);
    for(int i = 0; i < 10; i++)
        poola.createComponent(i);

    auto view = poola.getComponentView();
    view.map([](auto& comp) {
        comp.print();
    });

    for(int i = 0; i < 1000; i++) {
        poola.deleteComponent(rand() % 10);
    }
    //gPool.deleteComponent<Component>(id);
}
