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

    typedef myFunction<int(int, int)> iii;

    iii empty;
    assert(!empty);

    iii null(nullptr);
    assert(!null);

    //iii f([](int a, int b) {return a + b;});
    iii f(pluss);
    assert(f);
    cout << f(1, 1) << "\n";

    auto copy(f);
    assert(copy);
    cout << f(1, 1) << " " << copy(2, 2) << "\n";

    auto move(std::move(f));
    assert(move);
    cout << move(1, 2) << " " << copy(2, 3) << "\n";

    empty = move;
    assert(empty);
    cout << move(1, 4) << " " << empty(4, 3) << "\n";

    null = std::move(empty);
    assert(null);
    //assert(!empty);
    cout << null(3, 5) << "\n";

    iii plus([](int a, int b) { return a + b; });
    iii mul([](int a, int b) { return a * b; });
    cout << plus(4, 4) << " " << mul(4, 4) << "\n";
    plus.swap(mul);
    cout << plus(4, 4) << " " << mul(4, 4) << "\n";

    Plus p = Plus();
    myFunction<int (int, int)> fff(p);
    std::function aou(p);
    cout << fff(3, 10) << " " << fff(3, 10) << "\n";

    std::function ff([](int &a, int b) { return a + b; });
    int a = 40;
    cout << ff(a, 10) << " " << ff(++a, 10);

    return 0;
}