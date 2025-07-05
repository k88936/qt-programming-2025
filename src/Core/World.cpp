//
// Created by kvtoD on 6/30/2025.
//

#include "World.h"

#include "../Systems/AnimatorSystem.h"
#include "../Systems/ScriptSystem.h"
#include "../Systems/KeyboardControlSystem.h"
#include "../Systems/PhysicsSystem.h"

void World::loop()
{
    // Update input first
    KeyboardControlSystem::getInstance().update();
    AnimatorSystem::getInstance().update();

    // Update scripts (which now include state management)
    ScriptSystem::getInstance().update();

    // Update physics after scripts have updated forces/impulses
    PhysicsSystem::getInstance().update();
}
