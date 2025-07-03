//
// Created by kvtoD on 6/30/2025.
//

#ifndef WORLD_H
#define WORLD_H
#include "Utils/Singletion.h"
#include "entt/entity/registry.hpp"
#include "Systems/KeyboardControlSystem.h"
#include "Systems/PhysicsSystem.h"
#include "Systems/StateMachineSystem.h"


class World final : public Singleton<World>  {
public:
    entt::registry registry;
    void loop();
};


#endif //WORLD_H
