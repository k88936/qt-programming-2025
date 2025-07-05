//
// Created on 7/4/25.
//

#ifndef ANIMATOR_H
#define ANIMATOR_H

#include <string>
#include <unordered_map>
#include <vector>
#include "State.h"

// Animation clip data structure
struct Clip
{
    std::string name; // Name of this animation
    std::string basePath; // Directory containing animation frames
    int frameCount = 0; // Number of frames in the animation
    float frameDuration = 0.16f; // Duration of each frame in seconds
    bool loop = true; // Whether the animation should loop

    // Constructor
    Clip() = default;

    Clip(std::string name, std::string basePath, int frameCount,
                  float frameDuration = 0.16f, bool loop = true)
        : name(std::move(name)), basePath(std::move(basePath)),
          frameCount(frameCount), frameDuration(frameDuration), loop(loop)
    {
    }
};

// Current animation playback state
struct AnimationState
{
    const Clip* clip = nullptr; // Current playing clip
    int currentFrame = 0; // Current frame index
    float elapsedTime = 0.0f; // Time accumulated for current frame
    bool isPlaying = false; // Whether animation is currently playing

    // Get the current frame index to display
    int getDisplayFrameIndex() const
    {
        return clip ? currentFrame % clip->frameCount : 0;
    }
};

// Animation component for entities
struct Animator
{
    // Map from entity state to animation clips
    std::unordered_map<StateType, Clip> stateAnimations;

    // Current animation state
    AnimationState current;
};

#endif //ANIMATOR_H
