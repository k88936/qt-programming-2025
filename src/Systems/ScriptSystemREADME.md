# ScriptSystem

## Overview

The ScriptSystem manages game scripts execution within the ECS architecture. It provides a centralized system for registering and updating script components attached to entities.

## Features

- **Script Registration**: Registers script types for execution
- **Entity Update**: Updates all script components on relevant entities
- **Automatic Integration**: Works with the ECS registry to find and update scripts

## Usage

### Registering Scripts

Scripts are automatically registered when added to an entity:

```
cpp
registry.emplace<PlayerScript>(entity);
```

### Script Execution

All registered scripts are executed during the game loop:

```
bash
ScriptSystem::getInstance().update();
```
## Integration
