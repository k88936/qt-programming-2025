//
// Created by kvtoD on 6/30/2025.
//

#ifndef WORLD_H
#define WORLD_H
#include "../Utils/Singletion.h"
#include "entt/entity/registry.hpp"


class World final : public Singleton<World>
{
public:
    entt::registry registry;
    void update();
    void init();
};


#endif //WORLD_H
