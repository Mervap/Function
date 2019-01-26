//
// Created by Valery on 19.01.2019.
//

#ifndef FUNCTION_MYFUNCTION_H
#define FUNCTION_MYFUNCTION_H

#include <iostream>
#include <algorithm>
#include <memory>

const size_t BUFF_SIZE = 16;

template<typename T>
struct myFunction;

template<typename Ret, typename ... Args>
struct myFunction<Ret(Args ...)> {
private:
    struct concept {
        concept() = default;

        virtual ~concept() = default;

        virtual std::unique_ptr<concept> copy() const = 0;

        virtual Ret invoke(Args &&... args) = 0;

        virtual void placementSmall(void *to) const = 0;

        virtual void movePlacementSmall(void *to) = 0;
    };

    template<typename F>
    struct model : concept {
        explicit model(F func) : concept(), func(std::move(func)) {}

        virtual std::unique_ptr<concept> copy() const {
            return std::make_unique<model<F>>(func);
        }

        virtual Ret invoke(Args &&...args) {
            return func(std::forward<Args>(args)...);
        }

        virtual void placementSmall(void *to) const {
            new(to) model<F>(func);
        }

        virtual void movePlacementSmall(void *to) {
            new(to) model<F>(std::move(func));
        }

    private:
        F func;
    };

public:
    myFunction() noexcept {}

    myFunction(std::nullptr_t) noexcept : myFunction() {}

    ~myFunction() {
        if (isSmall) {
            reinterpret_cast<concept *>(st)->~concept();
        } else {
            ptr.~unique_ptr();
        }
    }

    template<typename F>
    explicit myFunction(F f) {
        if (sizeof(F) <= BUFF_SIZE && std::is_nothrow_move_constructible<F>::value) {
            isSmall = true;
            new(st) model<F>(std::move(f));
        } else {
            isSmall = false;
            ptr = std::make_unique<model<F>>(std::move(f));
        }
    }

    myFunction(const myFunction &other) {
        if (other.isSmall) {
            reinterpret_cast<const concept *>(other.st)->placementSmall(st);
        } else {
            ptr = other.ptr->copy();
        }
        isSmall = other.isSmall;
    }

    myFunction(myFunction &&other) noexcept {
        isSmall = other.isSmall;
        if (other.isSmall) {
            reinterpret_cast<concept *>(other.st)->movePlacementSmall(st);
            reinterpret_cast<concept *>(other.st)->~concept();
            new(&other.ptr) std::unique_ptr<concept>(nullptr);
            other.isSmall = false;
        } else {
            ptr = std::move(other.ptr);
        }
    }

    myFunction &operator=(const myFunction &other) {
        myFunction tmp(other);
        swap(tmp);
        return *this;
    }

    myFunction &operator=(myFunction &&other) noexcept {
        if (isSmall) {
            reinterpret_cast<concept *>(st)->~concept();
        } else {
            ptr.~unique_ptr();
        }

        isSmall = other.isSmall;

        if (other.isSmall) {
            reinterpret_cast<concept *>(other.st)->movePlacementSmall(st);
            reinterpret_cast<concept *>(other.st)->~concept();
            new(&other.ptr) std::unique_ptr<concept>(nullptr);
            other.isSmall = false;
        } else {
            new(&ptr) std::unique_ptr<concept>(std::move(other.ptr));
        }
        return *this;
    }

    Ret operator()(Args &&... args) {
        if (isSmall) {
            return reinterpret_cast<concept *>(st)->invoke(std::forward<Args>(args)...);
        }
        return ptr->invoke(std::forward<Args>(args)...);
    }

    explicit operator bool() const noexcept {
        return isSmall || ptr;
    }

    void swap(myFunction &other) noexcept {
        if (other.isSmall == isSmall) {
            if (isSmall) {
                std::swap(st, other.st);
            } else {
                std::swap(ptr, other.ptr);
            }
        } else if (other.isSmall) {
            auto tmp = std::move(ptr);
            ptr.~unique_ptr();
            reinterpret_cast<concept *>(other.st)->movePlacementSmall(st);
            reinterpret_cast<concept *>(other.st)->~concept();
            new(&other.ptr) std::unique_ptr<concept>(std::move(tmp));
        } else {
            auto tmp = std::move(other.ptr);
            other.ptr.~unique_ptr();
            reinterpret_cast<concept *>(st)->movePlacementSmall(other.st);
            reinterpret_cast<concept *>(st)->~concept();
            new(&ptr) std::unique_ptr<concept>(std::move(tmp));
        }

        std::swap(isSmall, other.isSmall);
    }


    union {
        std::unique_ptr<concept> ptr = nullptr;
        char st[BUFF_SIZE];
    };

    bool isSmall = false;
};

#endif //FUNCTION_MYFUNCTION_H
