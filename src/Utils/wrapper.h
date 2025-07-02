//
// Created by kvtoD on 7/1/2025.
//

#ifndef WRAPPER_H
#define WRAPPER_H
#include <cstdint>

#include "entt/entity/entity.hpp"

template<typename T>
class Wrapper {
private:
    T wrapped;

public:
    Wrapper(void *userData): wrapped(static_cast<T>(reinterpret_cast<std::uintptr_t>(userData))) {
        static_assert(sizeof(T) == sizeof(void *));
    }

    Wrapper(const T &e) : wrapped(e) {
    }

    operator T() const {
        return wrapped;
    }

    operator void *() const {
        static_assert(sizeof(T) == sizeof(void *));
        return reinterpret_cast<void *>(static_cast<std::uintptr_t>(wrapped));
    }
};

typedef Wrapper<entt::entity> EntityWrapper;

#endif //WRAPPER_H
