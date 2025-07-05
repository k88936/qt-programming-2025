//
// Created by kvtoD on 6/30/2025.
//

#ifndef SYSTEM_H
#define SYSTEM_H
#include "../Utils/Singletion.h"
#include "entt/entity/registry.hpp"

template<typename S>
class System : public Singleton<S>{
    virtual void update(){};
    virtual void init(){};
};

#endif //SYSTEM_H
