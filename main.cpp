#include <iostream>
#include <cassert>
#include <functional>
#include "myFunction.h"

#include <xmmintrin.h>

using namespace std;

//Small obj
int pluss(int a, int b) {
    return a + b;
}

int minuss(int a, int b, int c, int d, int e, int f, int l) {
    return a - b + c - d + e - f + l;
}

//Big obj
struct Plus {

    Plus() = default;

    Plus(const Plus &other) = default;

    Plus &operator=(const Plus &other) = default;

    Plus &operator=(Plus &&other) = default;

    Plus(Plus &&other) {
        if (cnt > 0) {
            throw std::runtime_error("Error");
        }
        ++cnt;
    }

    int operator()(int a, int b) {
        return a + b;
    }

private:
    int cnt = 0;
};

struct Minus {

    Minus() = default;

    Minus(const Minus &other) = default;

    Minus &operator=(const Minus &other) = default;

    Minus &operator=(Minus &&other) = default;

    Minus(Minus &&other) {}

    int operator()(int a, int b) {
        return a - b;
    }
};


struct sse_func {
    sse_func()
            : data() {}

    void operator()() const {}

    __m128 data;
};

template<typename T, typename ... Args>
std::unique_ptr<T> make_uniquee(Args &&... args) {
    return unique_ptr<T>(new T(std::forward<Args>(args)...));
}

struct stringHolder {

    stringHolder(const string &a) : a(a) {}

    std::string a;
};


template<typename F>
F&& max(F &&arg) {
    return std::forward<F>(arg);
}

template<typename F, typename ... T>
decltype(auto) max(F &&f, T &&... args) {
    auto tmp = max(std::forward<T>(args)...);
    return f > tmp ? std::forward<F>(f) : std::move(tmp);
}


int main() {
    typedef myFunction<int(int, int)> iii;

    //Empty
    iii empty;
    assert(!empty);

    //Null
    iii null(nullptr);
    assert(!null);

    //Constructor
    iii f(pluss);
    assert(f);
    cout << f(1, 1) << "\n", assert(f(1, 1) == 2);

    //Copy constructor
    auto copy(f);
    assert(copy);
    cout << f(1, 1) << " " << copy(2, 2) << "\n", assert(f(1, 1) == 2 && copy(2, 2) == 4);

    //Move constuctor
    auto move(std::move(f));
    assert(move);
    assert(!f);
    cout << move(1, 2) << " " << copy(2, 3) << "\n", assert(move(1, 2) == 3 && copy(2, 3) == 5);

    //operator=
    empty = move;
    assert(empty);
    cout << move(1, 4) << " " << empty(4, 3) << "\n", assert(move(1, 4) == 5 && empty(4, 3) == 7);

    //move operator=
    null = std::move(empty);
    assert(null);
    assert(!empty);
    cout << null(3, 5) << "\n", assert(null(3, 5) == 8);

    //Lambdas + swap with equal isSmall
    iii plus([](int a, int b) { return a + b; });
    iii mul([](int a, int b) { return a * b; });
    cout << plus(4, 4) << " " << mul(4, 4) << "\n", assert(plus(4, 4) == 8 && mul(4, 4) == 16);
    plus.swap(mul);
    cout << plus(4, 4) << " " << mul(4, 4) << "\n", assert(plus(4, 4) == 16 && mul(4, 4) == 8);

    //Big obj + swap with diff isSmall
    Plus p = Plus();
    iii fff(p);
    iii mulf([](int a, int b) { return a * b + 1; });
    cout << fff(3, 3) << " " << mulf(3, 3) << "\n", assert(fff(3, 3) == 6 && mulf(3, 3) == 10);
    fff.swap(mulf);
    cout << fff(3, 3) << " " << mulf(3, 3) << "\n", assert(fff(3, 3) == 10 && mulf(3, 3) == 6);

    //Forward eval
    myFunction<int(int &, int)> ff([](int &a, int b) { return a + b; });
    int a = 40;
    cout << ff(a, 10) << " ", assert(ff(a, 10) == 50), cout << ff(++a, 10) << "\n", assert(ff(a, 10) == 51);

    //Many args eval
    typedef myFunction<int(int, int, int, int, int, int, int)> iiii;
    iiii minn(minuss);
    cout << minn(1, 2, 3, 4, 5, 6, 7) << "\n", assert(minn(1, 2, 3, 4, 5, 6, 7) == 4);


    //Big obj swap
    Plus p1;
    Minus p2;
    iii fp1(p1);
    iii fp2(p2);
    assert(fp1 && fp2);

    cout << fp1(1, 1) << " " << fp2(1, 2) << "\n", assert(fp1(1, 1) == 2 && fp2(1, 2) == -1);
    fp1.swap(fp2);
    cout << fp1(1, 1) << " " << fp2(1, 2) << "\n", assert(fp1(1, 1) == 0 && fp2(1, 2) == 3);

    //Big move constructor
    iii fp3(std::move(fp1));
    assert(fp3);
    assert(!fp1);
    cout << fp3(1, 1) << "\n", assert(fp3(1, 1) == 0 && !fp1);

    std::cout << alignof(__m128) << " " << sizeof(__m128) << std::endl;
    myFunction<void()> f1((sse_func()));

    f1();

    const string s("Alala");
    cout << s << "\n";
    auto po = make_uniquee<stringHolder>(s);
    cout << s << "\n";
    cout << (*po).a << "\n";

    int yy = 5;
    cout << max(yy) << "\n";
    cout << max(1, 2.0, 3.5f, -10ll);
    return 0;
}