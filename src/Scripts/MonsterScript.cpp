//
// Created on 7/5/25.
//

#include "MonsterScript.h"
#include "../Core/World.h"
#include "../Managers/EventManager.h"
#include "../Components/Transform.h"

#include <iostream>

MonsterScript::MonsterScript()
{
    // Subscribe to ground contact events
    EventManager::getInstance().dispatcher.sink<GroundContactEvent>().connect<&MonsterScript::handleGroundContact>(this);
}

MonsterScript::~MonsterScript()
{
    // Disconnect from events when script is destroyed
    EventManager::getInstance().dispatcher.sink<GroundContactEvent>().disconnect<&MonsterScript::handleGroundContact>(this);
}

void MonsterScript::update(entt::entity entity)
{
    auto& registry = World::getInstance().registry;

    // Only process entities with required components
    if (!registry.all_of<Output, State>(entity)) {
        return;
    }

    // Get components
    auto& output = registry.get<Output>(entity);
    auto& state = registry.get<State>(entity);

    // Reset output forces/impulses
    output.reset();

    // Create AI-driven input
    Input aiInput = {};
    generateAIInput(entity, aiInput);

    // Update entity state based on AI logic
    updateEntityState(entity, state);

    // Apply output based on current state
    applyStateBasedOutput(state, output, entity);
}

void MonsterScript::handleGroundContact(const GroundContactEvent& event)
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
            StateType currentState = state.currentState;
            state.currentState = Idle;

            // Emit state change event
            EventManager::getInstance().dispatcher.enqueue<StateChangeEvent>(
                StateChangeEvent{.entity = event.entity, 
                                 .previousState = currentState, 
                                 .newState = state.currentState, 
                });
        }
        // If entity just left the ground and isn't jumping
        else if (!event.inContact && state.currentState != Jump)
        {
            StateType currentState = state.currentState;
            state.currentState = Falling;

            // Emit state change event
            EventManager::getInstance().dispatcher.enqueue<StateChangeEvent>(
                StateChangeEvent{.entity = event.entity, 
                                 .previousState = currentState, 
                                 .newState = state.currentState, 
                });
        }
    }
}

bool MonsterScript::isGrounded(entt::entity entity)
{
    // Check if entity is in our tracking map and is grounded
    auto it = groundedEntities.find(entity);
    if (it != groundedEntities.end())
    {
        return it->second;
    }

    // Default to true for entities we're not tracking yet
    groundedEntities[entity] = true;
    return true;
}

void MonsterScript::generateAIInput(entt::entity entity, Input& aiInput)
{
    auto& registry = World::getInstance().registry;

    // Find player position (this is a simple example - could be more sophisticated)
    Vector playerPos;
    Vector monsterPos;

    bool foundPlayer = false;
    bool hasTransform = registry.all_of<Transform>(entity);

    if (hasTransform) {
        monsterPos = registry.get<Transform>(entity).matrix.getPosition();

        // Find the player (first entity with PlayerScript)
        registry.view<PlayerScript>().each([&](auto playerEntity) {
            if (registry.all_of<Transform>(playerEntity)) {
                playerPos = registry.get<Transform>(playerEntity).matrix.getPosition();
                foundPlayer = true;
            }
        });
    }

    // If we found player and have transforms, make AI decisions
    if (foundPlayer && hasTransform) {
        float distance = playerPos.x - monsterPos.x;
        float absDistance = std::abs(distance);

        // Set AI inputs based on player position
        if (absDistance < attackRange) {
            // Player is in attack range
            aiInput.attack = true;
        } else if (absDistance < detectionRange) {
            // Player is detected but not in attack range - move toward player
            aiInput.left = distance < 0;
            aiInput.right = distance > 0;

            // Occasionally jump if grounded
            if (isGrounded(entity) && rand() % 100 < 5) {
                aiInput.up = true;
            }
        } else {
            // Random patrol behavior
            int randomMove = rand() % 100;
            if (randomMove < 30) {
                aiInput.left = true;
            } else if (randomMove < 60) {
                aiInput.right = true;
            }

            // Occasionally jump
            if (isGrounded(entity) && rand() % 100 < 2) {
                aiInput.up = true;
            }
        }
    }
}

void MonsterScript::updateEntityState(const entt::entity entity, State& state)
{
    auto& registry = World::getInstance().registry;

    // AI-driven state transitions
    const StateType previousState = state.currentState;

    // Create AI input for state transitions
    Input aiInput = {};
    generateAIInput(entity, aiInput);

    // State transitions based on AI input
    switch (previousState)
    {
    case Idle:
        if (aiInput.left || aiInput.right)
        {
            state.currentState = Moving;
        }
        else if (aiInput.up)
        {
            state.currentState = Jump;
        }
        else if (aiInput.attack)
        {
            state.currentState = Attack;
        }
        break;

    case Moving:
        if (!(aiInput.left || aiInput.right))
        {
            state.currentState = Idle;
        }
        else if (aiInput.up)
        {
            state.currentState = Jump;
        }
        else if (aiInput.attack)
        {
            state.currentState = Attack;
        }
        break;

    case Jump:
        if (isGrounded(entity))
        {
            if (aiInput.left || aiInput.right)
            {
                state.currentState = Moving;
            }
            else
            {
                state.currentState = Idle;
            }
        }
        break;

    case Falling:
        if (isGrounded(entity))
        {
            if (aiInput.left || aiInput.right)
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
        if (aiInput.left || aiInput.right)
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
        state.currentState = Idle;
        break;

    case Dead:
        // Dead state is terminal, no transitions out
        break;
    }

    // If state changed, emit a state change event
    if (previousState != state.currentState)
    {
        EventManager::getInstance().dispatcher.trigger<StateChangeEvent>(
            StateChangeEvent{.entity = entity, 
                             .previousState = previousState, 
                             .newState = state.currentState, 
            });
    }
}

void MonsterScript::applyStateBasedOutput(const State& state, Output& output, entt::entity entity)
{
    auto& registry = World::getInstance().registry;
    bool hasTransform = registry.all_of<Transform>(entity);
    Transform* transform = hasTransform ? &registry.get<Transform>(entity) : nullptr;

    // Create AI input for movement
    Input aiInput = {};
    generateAIInput(entity, aiInput);

    switch (state.currentState)
    {
    case Idle:
        // No forces in idle state
        break;

    case Moving:
        // Apply horizontal movement force
        if (aiInput.left)
        {
            if (transform) transform->matrix.updateFlip(-1);
            output.force = {-MOVE_FORCE, 0};
        }
        if (aiInput.right)
        {
            if (transform) transform->matrix.updateFlip(1);
            output.force = {MOVE_FORCE, 0};
        }
        break;

    case Jump:
        // Apply jump impulse
        output.impulse = {0.0f, JUMP_IMPULSE};

        // Allow horizontal control during jump
        if (aiInput.left)
        {
            output.force = {-MOVE_FORCE * 0.8f, 0};
        }
        if (aiInput.right)
        {
            output.force = {MOVE_FORCE * 0.8f, 0};
        }
        break;

    case Falling:
        // Allow some horizontal control during fall
        if (aiInput.left)
        {
            output.force = {-MOVE_FORCE * 0.5f, 0};
        }
        if (aiInput.right)
        {
            output.force = {MOVE_FORCE * 0.5f, 0};
        }
        break;

    case Attack:
        // Apply a small force in the attack direction for visual effect
        if (aiInput.left)
        {
            output.force = {-ATTACK_FORCE, 0};
        }
        else if (aiInput.right)
        {
            output.force = {ATTACK_FORCE, 0};
        }
        break;

    case Hurt:
        // No control during hurt state
        break;

    case Dead:
        // No control during dead state
        break;
    }
}
