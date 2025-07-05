//
// Created by root on 7/5/25.
//

#include "ScriptSystem.h"
#include "../Scripts/PlayerScript.h"

void ScriptSystem::init()
{
    for (const auto& script : initScripts)
    {
        script();
    }
}

void ScriptSystem::update()
{
    for (const auto& script : updateScripts)
    {
        script();
    }
}
