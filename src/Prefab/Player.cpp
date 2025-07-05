//
// Created by kvtoD on 6/30/2025.
//

#include "Player.h"

#include "Texture.h"
#include "../World.h"
#include "../Components/Drawable.h"
#include "../Components/Keymap.h"
#include "../Components/Input.h"
#include "../Components/Output.h"
#include "../Components/PhysicsDesciption.h"
#include "../Components/State.h"
#include "../Components/Animation.h"
#include "../Components/Tags.h"
#include "../Events/BodyEvents.h"
#include "../Events/KeyEvents.h"
#include "../Systems/AnimatorSystem.h"
#include "../Managers/EventManager.h"
#include "../Scripts/PlayerScript.h"

struct Input;
struct Keymap;

entt::entity Player::build(const Matrix& transform)
{
    const auto entity = Prefab::build(transform);
    auto& registry = World::getInstance().registry;

    // Add physics components
    registry.emplace<TagCreation>(entity);
    registry.emplace<PhysicsDes_CapsuleShapeDesc>(entity, PhysicsDes_CapsuleShapeDesc{
                                                      .halfHeight = 0.8f,
                                                      .radius = 0.5f,
                                                      .material = {.friction = 0.1f}
                                                  });
    registry.emplace<PhysicsDes_Movement>(entity, PhysicsDes_Movement{
                                              .type = PhysicsDes_Movement::Dynamic,
                                              .linearDamping = 0.5f
                                          });

    // Add input components
    registry.emplace<Keymap>(entity, Keymap{Key::Key_A, Key::Key_D, Key::Key_W, Key::Key_S, Key::Key_F});
    registry.emplace<Input>(entity);
    registry.emplace<Output>(entity);

    // Add state and rendering components
    registry.emplace<State>(entity);
    registry.emplace<Drawable>(entity, Drawable{.texture = nullptr});
    registry.emplace<Animation>(entity);
    registry.emplace<PlayerScript>(entity);
    initializeAnimations(entity);

    return entity;
}

void Player::initializeAnimations(entt::entity entity)
{
    // Get the animator system instance
    auto& animator = AnimatorSystem::getInstance();
    // Register animations for different states
    animator.registerAnimation(entity, Idle, "assets/player/idle", 0.2f, true);
    animator.registerAnimation(entity, Moving, "assets/player/move", 0.15f, true);

    // Optional: register additional animations if assets exist
    // animator.registerAnimation(entity, Jump, "assets/player/jump", 0.1f, false);
    // animator.registerAnimation(entity, Falling, "assets/player/fall", 0.1f, true);
    // animator.registerAnimation(entity, Attack, "assets/player/attack", 0.08f, false);

    // Set initial animation state based on entity's state
    auto& registry = World::getInstance().registry;
    if (registry.all_of<State>(entity))
    {
        auto& state = registry.get<State>(entity);
        switch (state.currentState)
        {
        case Moving:
            animator.play(entity, "move");
            break;
        case Idle:
        default:
            animator.play(entity, "idle");
            break;
        }
    }
    else
    {
        // Default to idle if no state component
        animator.play(entity, "idle");
    }
}
