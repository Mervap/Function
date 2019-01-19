#include <iostream>
#include <cassert>
#include <functional>
#include "myFunction.h"

using namespace std;

int pluss(int a, int b) {
    return a + b;
}

struct Plus {
    int operator()(int a, int b) {
        return a + b;
    }
};

int main() {

    typedef myFunction<int (int, int)> iii;

    iii empty;

    iii null(nullptr);

    //iii f([](int a, int b) {return a + b;});
    iii f(pluss);
    cout << f(1, 1) << "\n";

    auto copy(f);
    cout << f(1, 1) << " " << copy(2, 2) << "\n";

    auto move(std::move(f));
    assert(!f);
    cout << move(1, 2) << " " << copy(2, 3) << "\n";

    empty = move;
    cout << move(1, 4) << " " << empty(4, 3) << "\n";

    null = std::move(empty);
    assert(!empty);
    cout << null(3, 5) << "\n";

    iii plus([](int a, int b) {return a + b;});
    iii mul([](int a, int b) {return a * b;});
    cout << plus(4, 4) << " " << mul(4, 4) << "\n";
    plus.swap(mul);
    cout << plus(4, 4) << " " << mul(4, 4) << "\n";


    myFunction fff(pluss);

    std::function ff([](int a, int b) {return a + b;});
    return 0;
}