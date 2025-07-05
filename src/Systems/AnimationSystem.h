//
// Created on 7/4/25.
//

#ifndef ANIMATORSYSTEM_H
#define ANIMATORSYSTEM_H

#include "System.h"
#include "../Core/World.h"
#include "../Components/Animator.h"
#include "../Components/State.h"
#include <string>

class AnimationSystem final : public System<AnimationSystem>
{
public:
    AnimationSystem();
    ~AnimationSystem() override;

    void update() override;

    // Register a state-based animation
    bool registerAnimation(entt::entity entity, StateType state,
                           const std::string& basePath,
                           float frameDuration = 0.16f, bool loop = true);

    // Play a specific animation by name
    void play(entt::entity entity, const std::string& stateName);

    // Stop current animation
    void stop(entt::entity entity);

    // Set animation speed multiplier
    void setSpeed(entt::entity entity, float speedMultiplier);

    // Check if an animation is currently playing
    bool isPlaying(entt::entity entity) const;

    // Get the name of the currently playing animation
    std::string getCurrentAnimationName(entt::entity entity) const;

    // Handle state change events
    void onStateChange(const StateChangeEvent& event);

private:
    // Update animation based on elapsed time
    void updateAnimation(entt::entity entity, Animator& anim, float deltaTime);

    // Update the drawable component with the current frame
    void updateDrawableTexture(entt::entity entity, const Animator& anim);

    // Map state types to animation names
    static std::string getAnimationNameForState(StateType state);

    // Last update time for calculating deltaTime
    long lastUpdateTime = 0.0f;
};

#endif //ANIMATORSYSTEM_H
