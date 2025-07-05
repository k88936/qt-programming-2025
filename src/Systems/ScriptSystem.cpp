//
// Created by root on 7/5/25.
//

#include "ScriptSystem.h"
#include "../Scripts/PlayerScript.h"
void ScriptSystem::update()
{
    for (const auto& script : scripts)
    {
        script();
    }
}
