# Animator System

## Overview

The Animator System provides frame-by-frame animation capabilities with automatic state machine integration. It's designed to be similar to Unity's Animator component but adapted for our ECS architecture.

## Components

### Animation

The Animation component stores:

- **State Animations Map**: Links game states to animation clips
- **Current Animation State**: Tracks playback progress
- **Direction**: For sprite flipping (1 = right, -1 = left)

### AnimationClip

Defines a single animation:

- **Name**: Identifier for the animation
- **Base Path**: Directory containing animation frames
- **Frame Count**: Total number of frames
- **Frame Duration**: Seconds per frame
- **Loop**: Whether to repeat the animation

## TextureManager

The TextureManager provides automatic frame indexing and caching:

- Automatically loads frames from directories
- Sorts filenames intelligently (0.png comes before 10.png)
- Caches textures for performance

## Features

- **State-Driven Animations**: Automatically plays animations based on entity state
- **Manual Control**: Can override state-driven animations with direct play calls
- **Frame Management**: Handles timing, looping, and transitions
- **Direction Support**: Changes sprite orientation based on movement direction

## Usage

### Registering Animations

```cpp
// Register animation for a state
AnimatorSystem::getInstance().registerAnimation(
    entity,            // Target entity
    Idle,              // State enum
    "assets/player/idle", // Directory with frames
    0.2f,              // Frame duration
    true               // Loop
);
```

### Playing Animations

```cpp
// Play animation by state name
AnimatorSystem::getInstance().play(entity, "move");

// Stop animation
AnimatorSystem::getInstance().stop(entity);
```

### Checking Animation State

```cpp
// Is an animation playing?
bool playing = AnimatorSystem::getInstance().isPlaying(entity);

// What animation is currently playing?
std::string animName = AnimatorSystem::getInstance().getCurrentAnimationName(entity);
```

### Flipping Sprites

```cpp
// Set direction (-1 for left, 1 for right)
AnimatorSystem::getInstance().setDirection(entity, -1);
```

## Integration with State Machine

The Animator System automatically responds to StateChangeEvent events. When an entity's state changes, the appropriate animation will play without any additional code.

## File Structure

Animation frames should follow a consistent naming pattern:

```
assets/player/idle/0.png, 1.png, 2.png...
assets/player/move/0.png, 1.png, 2.png...
```

The TextureManager will sort them correctly regardless of leading zeros or prefixes.
