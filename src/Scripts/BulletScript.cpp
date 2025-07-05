//
// Created on 7/5/25.
//

#include "BulletScript.h"
#include "../Core/World.h"
#include "../Components/Transform.h"
#include <chrono>

BulletScript::BulletScript(const Vector& direction, float speed, float lifetime)
    : direction(direction), speed(speed), lifetime(lifetime)
{
    // Normalize direction vector
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    if (length > 0) {
        this->direction.x /= length;
        this->direction.y /= length;
    }
}

void BulletScript::update(entt::entity entity)
{
    auto& registry = World::getInstance().registry;

    // Calculate delta time
    static auto lastTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
    lastTime = currentTime;

    // Update lifetime
    elapsedTime += deltaTime;
    if (elapsedTime >= lifetime) {
        // Destroy bullet when lifetime expires
        registry.destroy(entity);
        return;
    }

    // Apply movement
    if (registry.all_of<Output>(entity)) {
        auto& output = registry.get<Output>(entity);
        output.force = {direction.x * speed, direction.y * speed};
    }

    // Update transform direction if needed
    if (registry.all_of<Transform>(entity)) {
        auto& transform = registry.get<Transform>(entity);
        // Set flip based on horizontal direction
        if (direction.x != 0) {
            transform.matrix.updateFlip(direction.x > 0 ? 1 : -1);
        }
    }
}
