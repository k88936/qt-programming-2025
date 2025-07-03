# StateMachine System for 2D Platformer

## Overview

The StateMachine system manages character states in a 2D platformer game using EnTT's entity-component-system architecture. It handles state transitions based on player input and physics conditions.

## Components

- **State**: Contains the current state of an entity (Idle, Moving, Jump, Falling, Attack, Dead, Hurt)
- **Input**: Player input commands (left, right, up, down, attack)
- **Output**: Forces and impulses to be applied to the entity by the physics system

## State Transitions

The system defines the following state transitions:

### Idle
- → Moving: When horizontal input is detected
- → Jump: When up input is detected
- → Attack: When attack input is detected

### Moving
- → Idle: When no horizontal input is detected
- → Jump: When up input is detected
- → Attack: When attack input is detected

### Jump
- → Falling: Automatic transition when apex of jump is reached (handled by physics)
- → Idle/Moving: When entity touches ground

### Falling
- → Idle/Moving: When entity touches ground

### Attack
- → Idle/Moving: When attack animation completes

### Hurt
- → Idle: When hurt animation completes

### Dead
- No transitions (terminal state)

## Events

The system uses the following events:

- **StateChangeEvent**: Triggered when an entity changes state
- **GroundContactEvent**: Triggered when an entity touches or leaves the ground

## Ground Detection

The system tracks which entities are grounded using a map. This information comes from physics collision events (not implemented in this example).

## Usage

To use the StateMachine system:

1. Add the required components to entities (Input, Output, State)
2. Include the StateMachineSystem in your game loop
3. Subscribe to state change events if needed

```cpp
// Example usage in main game loop
StateMachineSystem::getInstance().update();
```
