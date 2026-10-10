# My Unreal Engine Playbook

This repository serves as a professional reference for core Unreal Engine systems, specifically focusing on the Gameplay Ability System (GAS) and StateTree AI.

## Key Systems

### 1. Gameplay Ability System (GAS)
- **BasicAttributeSet**: Core stats including Health, MaxHealth, Stamina, MaxStamina, and Damage. Includes network replication support.
- **MeleeAttributeSet**: Combat-specific attributes like Attack Power, Defense, and Attack Speed.
- **MyCharacterBase**: Base character class integrating AbilitySystemComponent and attribute sets.

### 2. Combat System
- **PlayerCombatComponent**: Decoupled combat logic that handles inputs via Gameplay Tags and manages damage application using Gameplay Effects.

### 3. AI & Perception
- **BaseGuardAIController**: Foundation for AI behavior using the StateTree system.
- **BaseNoiseEmitter**: A specialized actor for handling AI hearing senses using `UAISense_Hearing` and `ReportNoiseEvent`.

## Usage
Use this repository as a reference when building new projects (like Lyra-based ones) to ensure consistent implementation of core logic.
