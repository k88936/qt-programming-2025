//
// Created by kvtoD on 6/30/2025.
//

#ifndef SINGLETION_H
#define SINGLETION_H
#include <iostream>

template<typename T>
class Singleton {
protected:
    Singleton() = default;

    virtual ~Singleton() = default;

public:
    // 禁止拷贝和赋值
    Singleton(const Singleton &) = delete;

    Singleton &operator=(const Singleton &) = delete;

    static T &getInstance() {
        static T instance;
        return instance;
    }
};
#endif //SINGLETION_H
