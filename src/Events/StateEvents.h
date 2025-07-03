//
// Created on 7/3/25.
//

#ifndef STATEEVENTS_H
#define STATEEVENTS_H

#include <entt/entity/entity.hpp>
#include "../Components/State.h"

// Event triggered when entity state changes
struct StateChangeEvent {
    entt::entity entity;
    StateType previousState;
    StateType newState;
};

// Event for special state transitions
struct GroundContactEvent {
    entt::entity entity;
    bool inContact;
};

#endif //STATEEVENTS_H
