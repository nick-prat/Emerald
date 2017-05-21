#include "../Emerald/component.hpp"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <unordered_map>
#include <algorithm>

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

    int getVal() const {
        return m_val;
    }

    void setVal(int val) {
        m_val = val;
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
    srand(time(0));
    ComponentPool<AComponent> poola(10);
    for(int i = 0; i < 10; i++)
        poola.createComponent(i);

    poola.deleteComponent(0);
    poola.deleteComponent(8);
    poola.deleteComponent(9);

    std::cout << "\nranged based for\n";
    const auto view = poola.getComponentView();
    for(auto& comp : view) {
        std::cout << comp.getVal() << '\n';
        //comp.setVal(10);
    }

    std::cout << "\nstd::for_each\n";
    std::for_each(view.begin(), view.end(), [](auto& comp) {
        std::cout << comp.getVal() << '\n';
    });

    std::cout << "\noperator++ and operator->\n";
    auto iter = poola.getComponentView().begin();
    std::cout << iter++->getVal() << '\n';
    std::cout << iter->getVal() << '\n';
    std::cout << (++iter)->getVal() << '\n';

    std::cout << "\nPoolView::map\n";
    view.map([](auto& comp) {
        comp.print();
    });

    for(int i = 0; i < 1000; i++) {
        poola.deleteComponent(rand() % 10);
    }
    //gPool.deleteComponent<Component>(id);
}
