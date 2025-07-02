//
// Created by kvtoD on 6/30/2025.
//

#include "Player.h"

#include "../World.h"
#include "../Compoments/Body.h"
#include "../Compoments/Drawable.h"
#include "../Compoments/Keymap.h"
#include "../Compoments/Input.h"
#include "../Compoments/Output.h"
#include "../Compoments/PhysicsDesciption.h"
#include "../Compoments/Tags.h"
#include "../Events/BodyEvents.h"
#include "../Managers/EventManager.h"
#include "box2d/box2d.h"
#include "box2d/types.h"

struct Input;
struct Keymap;

entt::entity Player::build(const Matrix &transform) {
    const auto entity = Prefab::build(transform);
    auto &registry = World::getInstance().registry;
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
    registry.emplace<Keymap>(entity, Keymap{Key_A, Key_D, Key_W, Key_S, Key_F});
    registry.emplace<Input>(entity);
    registry.emplace<Drawable>(entity);
    registry.emplace<Output>(entity);
    return entity;
}
