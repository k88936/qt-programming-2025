//
// Created on 7/3/25.
//

#include "StateMachineSystem.h"
#include "../Events/BodyEvents.h"
#include <unordered_map>

#include "../Managers/EventManager.h"

StateMachineSystem::StateMachineSystem()
{
    // Subscribe to ground contact events
    EventManager::getInstance().dispatcher.sink<GroundContactEvent>().connect<&
        StateMachineSystem::handleGroundContact>(this);
}

StateMachineSystem::~StateMachineSystem()
{
    // Disconnect from events when system is destroyed
    EventManager::getInstance().dispatcher.sink<GroundContactEvent>().disconnect<&
        StateMachineSystem::handleGroundContact>(this);
}

void StateMachineSystem::handleGroundContact(const GroundContactEvent& event)
{
    // Update grounded status in our tracking map
    groundedEntities[event.entity] = event.inContact;

    // If entity has a State component, update it appropriately
    auto& registry = World::getInstance().registry;
    if (registry.all_of<State>(event.entity))
    {
        auto& state = registry.get<State>(event.entity);

        // If entity just landed on ground
        if (event.inContact && (state.currentState == Jump || state.currentState == Falling))
        {
            // Land on ground - transition to idle or moving based on input
            if (registry.all_of<Input>(event.entity))
            {
                const auto& input = registry.get<Input>(event.entity);
                StateType currentState = state.currentState;

                if (isMovingHorizontally(input))
                {
                    state.currentState = Moving;
                }
                else
                {
                    state.currentState = Idle;
                }

                // Emit state change event
                EventManager::getInstance().dispatcher.enqueue<StateChangeEvent>(
                    event.entity, currentState, state.currentState);
            }
            else
            {
                state.currentState = Idle;
            }
        }
        // If entity just left the ground and isn't jumping
        else if (!event.inContact && state.currentState != Jump)
        {
            StateType currentState = state.currentState;
            state.currentState = Falling;

            // Emit state change event
            EventManager::getInstance().dispatcher.enqueue<StateChangeEvent>(
                event.entity, currentState, state.currentState);
        }
    }
}

// Movement constants
namespace
{
    constexpr float MOVE_FORCE = 25.0f;
    constexpr float JUMP_IMPULSE = 25.0f;
    constexpr float ATTACK_FORCE = 10.0f;
}

void StateMachineSystem::update()
{
    auto& registry = World::getInstance().registry;

    // Get all entities with required components for state machine
    auto view = registry.view<Input, Output, State>();

    view.each([this](const auto entity, const auto& input, auto& output, auto& state)
    {
        // Reset output forces/impulses
        output.reset();
        // Update entity state based on input and current conditions
        updateEntityState(entity, input, output, state);
        // Apply output based on current state
        applyStateBasedOutput(state, input, output);
    });
}

bool StateMachineSystem::isGrounded(entt::entity entity)
{
    // Check if entity is in our tracking map and is grounded
    auto it = groundedEntities.find(entity);
    if (it != groundedEntities.end())
    {
        return it->second;
    }

    // Default to true for entities we're not tracking yet
    // In a complete implementation, this would use a physics system
    // to determine the initial state
    groundedEntities[entity] = true;
    return true;
}

bool StateMachineSystem::isMovingHorizontally(const Input& input)
{
    return input.left || input.right;
}

void StateMachineSystem::updateEntityState(entt::entity entity, const Input& input, Output& output, State& state)
{
    // Previous state for transition logic
    StateType previousState = state.currentState;

    // State transitions
    switch (previousState)
    {
    case Idle:
        if (isMovingHorizontally(input))
        {
            state.currentState = Moving;
        }
        else if (input.up)
        {
            state.currentState = Jump;
        }
        else if (input.attack)
        {
            state.currentState = Attack;
        }
        break;

    case Moving:
        if (!isMovingHorizontally(input))
        {
            state.currentState = Idle;
        }
        else if (input.up)
        {
            state.currentState = Jump;
        }
        else if (input.attack)
        {
            state.currentState = Attack;
        }
        break;

    case Jump:
        // Transition to falling state would be handled by physics system
        // For now, we'll go back to idle after jump
        if (isGrounded(entity))
        {
            if (isMovingHorizontally(input))
            {
                state.currentState = Moving;
            }
            else
            {
                state.currentState = Idle;
            }
        }
        else
        {
            // Would transition to Falling in a complete implementation
        }
        break;

    case Falling:
        if (isGrounded(entity))
        {
            if (isMovingHorizontally(input))
            {
                state.currentState = Moving;
            }
            else
            {
                state.currentState = Idle;
            }
        }
        break;

    case Attack:
        // Attack state is temporary, return to previous state
        // In a real implementation, this would be tied to animation completion
        if (isMovingHorizontally(input))
        {
            state.currentState = Moving;
        }
        else
        {
            state.currentState = Idle;
        }
        break;

    case Hurt:
        // Hurt state is temporary, return to idle after a short time
        // In a real implementation, this would be tied to a timer
        state.currentState = Idle;
        break;

    case Dead:
        // Dead state is terminal, no transitions out
        break;
    }

    // If state changed, emit a state change event
    if (previousState != state.currentState)
    {
        EventManager::getInstance().dispatcher.enqueue<StateChangeEvent>(
            StateChangeEvent{.entity = entity, .previousState = previousState, .newState = state.currentState});
    }
}

void StateMachineSystem::applyStateBasedOutput(const State& state, const Input& input, Output& output)
{
    switch (state.currentState)
    {
    case Idle:
        // No forces in idle state
        break;

    case Moving:
        // Apply horizontal movement force
        if (input.left)
        {
            output.force = {-MOVE_FORCE, 0};
        }
        if (input.right)
        {
            output.force = {MOVE_FORCE, 0};
        }
        break;

    case Jump:
        // Apply jump impulse
        output.impulse = {0.0f, JUMP_IMPULSE};

        // Allow horizontal control during jump
        if (input.left)
        {
            output.force = {-MOVE_FORCE * 0.8f, 0};
        }
        if (input.right)
        {
            output.force = {MOVE_FORCE * 0.8f, 0};
        }
        break;

    case Falling:
        // Allow some horizontal control during fall
        if (input.left)
        {
            output.force = {-MOVE_FORCE * 0.5f, 0};
        }
        if (input.right)
        {
            output.force = {MOVE_FORCE * 0.5f, 0};
        }
        break;

    case Attack:
        // Apply a small force in the attack direction for visual effect
        if (input.left)
        {
            output.force = {-ATTACK_FORCE, 0};
        }
        else if (input.right)
        {
            output.force = {ATTACK_FORCE, 0};
        }
        break;

    case Hurt:
        // No player control during hurt state
        break;

    case Dead:
        // No player control during dead state
        break;
    }
}
