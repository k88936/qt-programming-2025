//
// Created on 7/3/25.
//

#ifndef STATEMACHINESYSTEM_H
#define STATEMACHINESYSTEM_H

#include "System.h"
#include "../World.h"
#include "../Components/Input.h"
#include "../Components/Output.h"
#include "../Components/State.h"
#include "../Components/PhysicsDesciption.h"
#include "../Events/StateEvents.h"
#include <entt/signal/dispatcher.hpp>

// Forward declarations
class BodyState;

class StateMachineSystem final : public System<StateMachineSystem> {
public:
    StateMachineSystem();
    ~StateMachineSystem() override;

    void update() override;

    // Event dispatcher for state change events
    void handleGroundContact(const GroundContactEvent& event);

private:
    // Track which entities are grounded
    std::unordered_map<entt::entity, bool> groundedEntities;
    // Check if entity is on ground
    bool isGrounded(entt::entity entity);
    // Check if entity is moving horizontally
    bool isMovingHorizontally(const Input& input);
    // Apply state transitions and update output
    void updateEntityState(entt::entity entity, const Input& input, Output& output, State& state);
    // Apply outputs based on current state
    void applyStateBasedOutput(const State& state, const Input& input, Output& output);
};

#endif //STATEMACHINESYSTEM_H
