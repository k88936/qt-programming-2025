//
// Created by kvtoD on 6/30/2025.
//

#include "World.h"

#include "../Prefab/Player.h"
#include "../Systems/AnimationSystem.h"
#include "../Systems/ScriptSystem.h"
#include "../Systems/KeyboardControlSystem.h"
#include "../Systems/PhysicsSystem.h"

void World::update()
{
    // Update input first
    KeyboardControlSystem::getInstance().update();
    AnimationSystem::getInstance().update();

    // Update scripts (which now include state management)
    ScriptSystem::getInstance().update();

    // Update physics after scripts have updated forces/impulses
    PhysicsSystem::getInstance().update();
}

void World::init()
{
    Player p;
    Matrix m;
    m.translate(0,1);
    p.build(m);
    ScriptSystem::getInstance().update();
}
