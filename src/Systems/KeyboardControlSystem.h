//
// Created by kvtoD on 6/30/2025.
//
#ifndef KEYBOARDCONTROLSYSTEM_H
#define KEYBOARDCONTROLSYSTEM_H
#include "../Managers/EventManager.h"
#include "System.h"
#include "entt/signal/dispatcher.hpp"
#include "../Events/KeyEvents.h"


class KeyboardControlSystem final : public System<KeyboardControlSystem> {
public:
    void update() override;

    void handlePressKeyEvent(const PressKey &event);

    void handleReleaseKeyEvent(const ReleaseKey &event);
    void cleanInput();

    KeyboardControlSystem();

    ~KeyboardControlSystem() override;
};


#endif //KEYBOARDCONTROLSYSTEM_H
