//
// Created by kvtoD on 6/30/2025.
//

#include "World.h"

#include "Systems/AnimatorSystem.h"
#include "Systems/ScriptSystem.h"

#include "Systems/KeyboardControlSystem.h"
#include "Systems/PhysicsSystem.h"
#include "Systems/StateMachineSystem.h"

void World::loop()
{
    KeyboardControlSystem::getInstance().update();
    StateMachineSystem::getInstance().update();
    AnimatorSystem::getInstance().update();
    PhysicsSystem::getInstance().update();
    ScriptSystem::getInstance().update();
}
