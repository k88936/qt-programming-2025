//
// Created by kvtoD on 6/30/2025.
//

#ifndef EVENTMANAGER_H
#define EVENTMANAGER_H
#include "../Utils/Singletion.h"
#include "entt/signal/dispatcher.hpp"

class EventManager final : public Singleton<EventManager>, public entt::dispatcher {
public:
    template<typename Type>
    inline void update() {
        entt::dispatcher::update<Type>();
    }

private:
    void update();
};


#endif //EVENTMANAGER_H
