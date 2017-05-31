#include <iostream>

using namespace std;

void func(int i) {
    cout << "int" << i << '\n';
}

void func(double d){
    cout << "double " << d << '\n';
}

template<typename T>
void func(T t) {
    cout << "other " << t << '\n';
}

int main() {
    func(1.0);
    func(1);
    func("Hi!");
}
