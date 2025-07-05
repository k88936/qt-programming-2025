//
// Created on 7/4/25.
//

#include "AnimatorSystem.h"
#include "../Managers/TextureManager.h"
#include <chrono>
#include <QDebug>

// Get current time in seconds
long getCurrentTimeMilliseconds()
{
    const auto now = std::chrono::high_resolution_clock::now();
    const auto duration = now.time_since_epoch();
    const long millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    return millis;
}

AnimatorSystem::AnimatorSystem()
{
    // Subscribe to state change events
    EventManager::getInstance().dispatcher.sink<StateChangeEvent>().connect<&AnimatorSystem::onStateChange>(this);
    lastUpdateTime = getCurrentTimeMilliseconds();
}

AnimatorSystem::~AnimatorSystem()
{
    // Disconnect from state change events
    EventManager::getInstance().dispatcher.sink<StateChangeEvent>().disconnect<&AnimatorSystem::onStateChange>(this);
}

void AnimatorSystem::update()
{
    // Calculate delta time
    const long currentTime = getCurrentTimeMilliseconds();
    float deltaTime = static_cast<float>(currentTime - lastUpdateTime) / 1000.0f;
    lastUpdateTime = currentTime;

    auto& registry = World::getInstance().registry;
    auto view = registry.view<Animation, Drawable>();

    view.each([this, deltaTime](auto entity, Animation& anim, Drawable& drawable)
    {
        // First ensure entity has a texture if animations are available
        if (drawable.texture == nullptr && !anim.stateAnimations.empty())
        {
            // Set initial texture from first available animation
            auto firstAnim = anim.stateAnimations.begin();
            if (firstAnim != anim.stateAnimations.end())
            {
                // Set current animation if not set
                if (!anim.current.clip)
                {
                    anim.current.clip = &firstAnim->second;
                    anim.current.currentFrame = 0;
                    anim.current.elapsedTime = 0.0f;
                    anim.current.isPlaying = true;
                }

                // Load initial texture
                updateDrawableTexture(entity, anim);
                qDebug() << "AnimatorSystem: Set initial texture for entity from animation";
            }
        }

        // Skip if no animation is playing
        if (!anim.current.isPlaying || !anim.current.clip)
        {
            return;
        }

        // Update animation frame
        updateAnimation(entity, anim, deltaTime);

        // Update the drawable texture
        updateDrawableTexture(entity, anim);
    });
}

bool AnimatorSystem::registerAnimation(entt::entity entity, StateType state,
                                       const std::string& basePath,
                                       float frameDuration, bool loop)
{
    auto& registry = World::getInstance().registry;

    // Check if directory exists
    if (!FileUtils::directoryExists(basePath))
    {
        qWarning() << "AnimatorSystem: Animation directory does not exist:"
            << QString::fromStdString(basePath);
        qDebug() << "Current working directory:"
            << QString::fromStdString(FileUtils::getAbsolutePath("."));
        return false;
    }

    // Ensure entity has Animation component
    if (!registry.all_of<Animation>(entity))
    {
        registry.emplace<Animation>(entity);
    }

    auto& anim = registry.get<Animation>(entity);

    // Get frame count from TextureManager
    int frameCount = TextureManager::getInstance().getTextureCount(basePath);
    if (frameCount <= 0)
    {
        qWarning() << "AnimatorSystem: No frames found in directory:" << QString::fromStdString(basePath);
        return false;
    }

    // Create animation clip
    const AnimationClip clip(getAnimationNameForState(state), basePath, frameCount, frameDuration, loop);

    // Register the animation
    anim.stateAnimations[state] = clip;

    // If this is the first animation, start playing it
    if (anim.current.clip == nullptr)
    {
        anim.current.clip = &anim.stateAnimations[state];
        anim.current.currentFrame = 0;
        anim.current.elapsedTime = 0.0f;
        anim.current.isPlaying = true;

        // Make sure the drawable has the first frame
        if (registry.all_of<Drawable>(entity))
        {
            updateDrawableTexture(entity, anim);
        }
    }

    return true;
}

void AnimatorSystem::play(entt::entity entity, const std::string& stateName)
{
    auto& registry = World::getInstance().registry;
    if (!registry.all_of<Animation>(entity))
    {
        return;
    }

    auto& anim = registry.get<Animation>(entity);

    // Find the animation by state name
    StateType targetState;
    if (stateName == "idle") targetState = Idle;
    else if (stateName == "move") targetState = Moving;
    else if (stateName == "jump") targetState = Jump;
    else if (stateName == "fall") targetState = Falling;
    else if (stateName == "attack") targetState = Attack;
    else if (stateName == "hurt") targetState = Hurt;
    else if (stateName == "dead") targetState = Dead;
    else
    {
        qWarning() << "AnimatorSystem: Unknown animation state:" << QString::fromStdString(stateName);
        return;
    }

    auto it = anim.stateAnimations.find(targetState);
    if (it != anim.stateAnimations.end())
    {
        // Don't restart if already playing this animation
        if (anim.current.clip == &it->second && anim.current.isPlaying)
        {
            return;
        }

        // Set up the current animation
        anim.current.clip = &it->second;
        anim.current.currentFrame = 0;
        anim.current.elapsedTime = 0.0f;
        anim.current.isPlaying = true;

        // Update drawable with the first frame
        if (registry.all_of<Drawable>(entity))
        {
            updateDrawableTexture(entity, anim);
        }
    }
    else
    {
        qWarning() << "AnimatorSystem: Animation not found for state:" << QString::fromStdString(stateName);
    }
}

void AnimatorSystem::stop(entt::entity entity)
{
    auto& registry = World::getInstance().registry;
    if (!registry.all_of<Animation>(entity))
    {
        return;
    }

    auto& anim = registry.get<Animation>(entity);
    anim.current.isPlaying = false;
}

void AnimatorSystem::setSpeed(entt::entity entity, float speedMultiplier)
{
    // Not implemented in this version
    // Would modify frameDuration of current clip
}

bool AnimatorSystem::isPlaying(entt::entity entity) const
{
    auto& registry = World::getInstance().registry;
    if (!registry.all_of<Animation>(entity))
    {
        return false;
    }

    const auto& anim = registry.get<Animation>(entity);
    return anim.current.isPlaying;
}

std::string AnimatorSystem::getCurrentAnimationName(entt::entity entity) const
{
    auto& registry = World::getInstance().registry;
    if (!registry.all_of<Animation>(entity))
    {
        return "";
    }

    const auto& anim = registry.get<Animation>(entity);
    return anim.current.clip ? anim.current.clip->name : "";
}

void AnimatorSystem::onStateChange(const StateChangeEvent& event)
{
    auto& registry = World::getInstance().registry;

    // Only handle entities with Animation component
    if (!registry.all_of<Animation>(event.entity))
    {
        return;
    }

    auto& anim = registry.get<Animation>(event.entity);

    // Find animation for the new state
    auto it = anim.stateAnimations.find(event.newState);
    if (it != anim.stateAnimations.end())
    {
        // Only change animation if different from current
        if (anim.current.clip != &it->second)
        {
            // Set up the current animation
            anim.current.clip = &it->second;
            anim.current.currentFrame = 0;
            anim.current.elapsedTime = 0.0f;
            anim.current.isPlaying = true;

            // Update drawable with the first frame
            if (registry.all_of<Drawable>(event.entity))
            {
                updateDrawableTexture(event.entity, anim);
            }
        }
    }
}

void AnimatorSystem::setDirection(entt::entity entity, int direction)
{
    auto& registry = World::getInstance().registry;
    if (!registry.all_of<Animation>(entity))
    {
        return;
    }

    auto& anim = registry.get<Animation>(entity);
    anim.direction = (direction >= 0) ? 1 : -1;
}

// void AnimatorSystem::initializeEntity(entt::entity entity)
// {
//     auto& registry = World::getInstance().registry;
//
//     // Make sure entity has required components
//     if (!registry.all_of<Drawable>(entity))
//     {
//         registry.emplace<Drawable>(entity, Drawable{.texture = nullptr});
//     }
//
//     if (!registry.all_of<Animation>(entity))
//     {
//         registry.emplace<Animation>(entity);
//     }
//
//     // Register default animations (assuming asset paths)
//     registerAnimation(entity, Idle, "assets/player/idle", 0.2f, true);
//     registerAnimation(entity, Moving, "assets/player/move", 0.15f, true);
//
//     // Make sure the idle animation is playing initially
//     auto& anim = registry.get<Animation>(entity);
//     auto it = anim.stateAnimations.find(Idle);
//     if (it != anim.stateAnimations.end())
//     {
//         anim.current.clip = &it->second;
//         anim.current.currentFrame = 0;
//         anim.current.elapsedTime = 0.0f;
//         anim.current.isPlaying = true;
//
//         // Update drawable with the first frame
//         updateDrawableTexture(entity, anim);
//     }
// }

void AnimatorSystem::updateAnimation(entt::entity entity, Animation& anim, float deltaTime)
{
    if (!anim.current.clip)
    {
        return;
    }

    // Add elapsed time
    anim.current.elapsedTime += deltaTime;

    // Check if we need to advance to next frame
    if (anim.current.elapsedTime >= anim.current.clip->frameDuration)
    {
        // How many frames to advance
        int framesToAdvance = static_cast<int>(anim.current.elapsedTime / anim.current.clip->frameDuration);
        anim.current.elapsedTime = fmod(anim.current.elapsedTime, anim.current.clip->frameDuration);

        // Handle frame advancement
        if (anim.current.clip->loop)
        {
            // For looping animations, wrap around
            anim.current.currentFrame = (anim.current.currentFrame + framesToAdvance) %
                anim.current.clip->frameCount;
        }
        else
        {
            // For non-looping animations, clamp at the end
            anim.current.currentFrame = std::min(anim.current.currentFrame + framesToAdvance,
                                                 anim.current.clip->frameCount - 1);

            // Stop animation when it reaches the end
            if (anim.current.currentFrame >= anim.current.clip->frameCount - 1)
            {
                anim.current.isPlaying = false;
            }
        }
    }
}

void AnimatorSystem::updateDrawableTexture(entt::entity entity, const Animation& anim)
{
    auto& registry = World::getInstance().registry;
    assert(registry.all_of<Drawable>(entity) && anim.current.clip);
    auto& drawable = registry.get<Drawable>(entity);

    // Get the current frame index
    int frameIndex = anim.current.getDisplayFrameIndex();

    // Get the texture from TextureManager
    Texture* texture = TextureManager::getInstance().getTexture(
        anim.current.clip->basePath, frameIndex);

    assert(texture);
    drawable.texture = texture;
}

std::string AnimatorSystem::getAnimationNameForState(StateType state)
{
    switch (state)
    {
    case Idle: return "idle";
    case Moving: return "move";
    case Jump: return "jump";
    case Falling: return "fall";
    case Attack: return "attack";
    case Hurt: return "hurt";
    case Dead: return "dead";
    default: return "unknown";
    }
}
