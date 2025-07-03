//
// Created by kvtoD on 6/30/2025.
//

#include "World.h"

void World::loop()
{
    KeyboardControlSystem::getInstance().update();
    StateMachineSystem::getInstance().update();
    PhysicsSystem::getInstance().update();
}
