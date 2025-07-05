//
// Created by root on 7/5/25.
//

#ifndef SCRIPT_H
#define SCRIPT_H
#include "entt/entity/entity.hpp"
#include "../Systems/ScriptSystem.h"
template <typename S>
class Script
{
public:
    virtual ~Script() = default;
    virtual void update(entt::entity) =0;

    Script();
};

template <typename S>
Script<S>::Script()
{
    static bool registered = false;
    if (!registered)
    {
        ScriptSystem::getInstance().registerScript<S>();
    }
}


#endif //SCRIPT_H
