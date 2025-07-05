//
// Created by root on 7/5/25.
//

#ifndef SCRIPTSYSTEM_H
#define SCRIPTSYSTEM_H
#include "System.h"
#include "../Core/World.h"

class ScriptSystem final : public System<ScriptSystem>
{
    std::vector<std::function<void()>> scripts;

public:
    template <typename S>
    void registerScript();

    void update() override;
};

template <typename S>
void ScriptSystem::registerScript()
{
    scripts.emplace_back([]()
    {
        World::getInstance().registry.view<S>().each([](auto entity, S& script)
        {
            script.update(entity);
        });
    });
}

#endif //SCRIPTSYSTEM_H
