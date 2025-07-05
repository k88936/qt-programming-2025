//
// Created by kvtoD on 6/30/2025.
//

#include "Prefab.h"

#include <qvariant.h>

#include "../Core/World.h"
#include "../Components/Transform.h"
#include "entt/entity/registry.hpp"

entt::entity Prefab::build(const Matrix &transform) {
    auto &registry = World::getInstance().registry;
    const auto entity = registry.create();
    registry.emplace<Transform>(entity, transform);
    return entity;
}
