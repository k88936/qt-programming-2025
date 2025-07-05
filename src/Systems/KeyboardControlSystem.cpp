//
// Created by kvtoD on 6/30/2025.
//

#include "KeyboardControlSystem.h"

#include "../Managers/EventManager.h"
#include "../Components/Input.h"
#include "../Components/Keymap.h"
#include "../Core/World.h"
#include "entt/entity/view.hpp"
#include "../Events/KeyEvents.h"
#include "../Utils/Dumper.h"


void KeyboardControlSystem::update()
{
    // cleanInput();
    // EventManager::getInstance().dispatcher.update<PressKey>();
    // EventManager::getInstance().dispatcher.update<ReleaseKey>();
    // dump<Input>();
}

void KeyboardControlSystem::handlePressKeyEvent(const PressKey& event)
{
    for (const auto view = World::getInstance().registry.view<const Keymap, Input>();
         const auto entity : view)
    {
        const auto& keymap = view.get<Keymap>(entity);
        if (event.key == keymap.keyLeft)
        {
            view.get<Input>(entity).left = true;
        }
        else if (event.key == keymap.keyRight)
        {
            view.get<Input>(entity).right = true;
        }
        else if (event.key == keymap.keyDown)
        {
            view.get<Input>(entity).down = true;
        }
        else if (event.key == keymap.keyUp)
        {
            view.get<Input>(entity).up = true;
        }
        else if (event.key == keymap.keyAttack)
        {
            view.get<Input>(entity).attack = true;
        }
    }
}

void KeyboardControlSystem::handleReleaseKeyEvent(const ReleaseKey& event)
{
    for (const auto view = World::getInstance().registry.view<const Keymap, Input>(); const auto entity : view)
    {
        const auto& keymap = view.get<Keymap>(entity);
        if (event.key == keymap.keyLeft)
        {
            view.get<Input>(entity).left = false;
        }
        else if (event.key == keymap.keyRight)
        {
            view.get<Input>(entity).right = false;
        }
        else if (event.key == keymap.keyDown)
        {
            view.get<Input>(entity).down = false;
        }
        else if (event.key == keymap.keyUp)
        {
            view.get<Input>(entity).up = false;
        }
        else if (event.key == keymap.keyAttack)
        {
            view.get<Input>(entity).attack = false;
        }
    }
}

void KeyboardControlSystem::cleanInput()
{
    auto view = World::getInstance().registry.view<Input>();
    view.each([](Input& input)
    {
        input.left = false;
        input.right = false;
        input.down = false;
        input.up = false;
        input.attack = false;
        input.attackOnce = false;
    });
}

KeyboardControlSystem::KeyboardControlSystem()
{
    EventManager::getInstance().dispatcher.sink<PressKey>().connect<&KeyboardControlSystem::handlePressKeyEvent>(this);
    EventManager::getInstance().dispatcher.sink<ReleaseKey>().connect<&
        KeyboardControlSystem::handleReleaseKeyEvent>(this);
}

KeyboardControlSystem::~KeyboardControlSystem()
{
    EventManager::getInstance().dispatcher.disconnect(this);
}
