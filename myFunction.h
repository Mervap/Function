//
// Created by Valery on 19.01.2019.
//

#ifndef FUNCTION_MYFUNCTION_H
#define FUNCTION_MYFUNCTION_H

#include <iostream>
#include <algorithm>
#include <memory>

template<typename T>
struct myFunction;

template<typename Ret, typename ... Args>
struct myFunction<Ret(Args ...)> {
    myFunction() noexcept : ptr(nullptr) {}

    myFunction(std::nullptr_t) noexcept : ptr(nullptr) {}

    ~myFunction() = default;

    template<typename F>
    myFunction(F f) : ptr(new model<F>(std::move(f))) {
    }

    myFunction(const myFunction<Ret(Args ...)> &other) : ptr(other.ptr->copy()) {}

    myFunction(myFunction &&other) noexcept : ptr(std::move(other.ptr)) {}

    myFunction &operator=(const myFunction &other) {
        ptr = other.ptr->copy();
        return *this;
    }

    myFunction &operator=(myFunction &&other) noexcept {
        ptr = std::move(other.ptr);
        return *this;
    }

    Ret operator()(Args ... args) {
        return ptr->invoke(args ...);
    }

    explicit operator bool() const noexcept {
        return ptr != nullptr;
    }

    void swap(myFunction &other) noexcept {
        ptr.swap(other.ptr);
    }

private:
    struct concept {
        concept() = default;

        virtual ~concept() = default;

        virtual std::unique_ptr<concept> copy() const = 0;

        virtual Ret invoke(Args ... args) = 0;
    };

    template<typename F>
    struct model : concept {
        explicit model(F func) : concept(), func(std::move(func)) {}

        std::unique_ptr<concept> copy() const {
            return std::make_unique<model<F>>(func);
        }

        virtual Ret invoke(Args ... args) {
            return func(args ...);
        }

    private:
        F func;
    };


    //bool isSmall;
    std::unique_ptr<concept> ptr;
    //Ret (*smallF)(Args...);
};

#endif //FUNCTION_MYFUNCTION_H
