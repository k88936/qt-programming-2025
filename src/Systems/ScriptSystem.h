//
// Created by root on 7/5/25.
//

#ifndef SCRIPTSYSTEM_H
#define SCRIPTSYSTEM_H
#include "System.h"
#include "../Core/World.h"

class ScriptSystem final : public System<ScriptSystem>
{
    std::vector<std::function<void()>> updateScripts;
    std::vector<std::function<void()>> initScripts;

public:
    template <typename... S>
    void registerScript();
    void init() override;

    void update() override;
};

template <typename... S>
void ScriptSystem::registerScript()
{
    using ScriptType = std::tuple_element_t<0, std::tuple<S...>>;
    updateScripts.emplace_back([]()
    {
        auto& registry = World::getInstance().registry;
        for (const auto view = registry.view<S...>(); const auto entity : view)
        {
            registry.get<ScriptType>(entity).update(entity);
        }
    });
    initScripts.emplace_back([]()
    {
        auto& registry = World::getInstance().registry;
        for (const auto view = registry.view<S...>(); const auto entity : view)
        {
            registry.get<ScriptType>(entity).init(entity);
        }
    });
}

#endif //SCRIPTSYSTEM_H
