## Concept
Harkc is a lightweight hierarchical state machine plugin for Unreal Engine that allows you to define state-driven logic using a fluent builder API. It is designed to simplify gameplay, UI, and system logic by replacing scattered conditionals with structured states and transitions.

## Installation
Note: Harkc requires C++ integration in your project to use.

1. Clone or download the repository.
2. Place the folder inside `<UnrealProject>/Plugins`.
3. Build the plugin through Visual Studio or by opening the project.

## Quick Start
To create and run a simple state machine:
```cpp
UHarkcMachine* Machine = UHarkcMachine::Create(this, "Example")
    ->Initial("Idle")
    ->State("Idle", [](FHarkcStateBuilder& S)
    {
        S.OnEntry([](FHarkcContext& C)
        {
            UE_LOG(LogTemp, Warning, TEXT("Entered Idle"));
        });

        S.On("Move", "Running");
    })
    ->State("Running", [](FHarkcStateBuilder& S)
    {
        S.On("Stop", "Idle");
    })
    ->Start();
```

To trigger a transition:
```cpp
Machine->Send("Move");
```

This transitions the machine from `Idle` -> `Running`.

## Core Usage

| Concept | Description |
|--------|------------|
| Create Machine | `UHarkcMachine::Create(Outer, "MachineId")` – creates a new machine instance |
| Set Initial State | `->Initial("StateName")` – defines the starting state |
| Define State | `->State(...)` – declares a state and its behavior |
| Add Transition | `S.On(Event, Target)` – triggers a state change |
| Start Machine | `->Start()` – begins execution |
| Send Event | `Machine->Send(Event)` – triggers transitions |

## Features
### OnEntry / OnExit
```cpp
S.OnEntry([](FHarkcContext& C) { ... });
S.OnExit([](FHarkcContext& C) { ... });
```
Runs when entering or exiting a state.

### Guards and Actions
```cpp
S.On("Attack", "Attacking")
 .Guard([](FHarkcContext& C) { return true; })
 .Action([](FHarkcContext& C) { /* side effects */ });
```
- Guards must pass for the transition to occur
- Actions run during the transition

### Nested (Hierarchical) States
```cpp
S.State("Menu", [](FHarkcStateBuilder& S)
{
    S.Initial("Main");

    S.State("Main", [](FHarkcStateBuilder& S) {});
    S.State("Options", [](FHarkcStateBuilder& S) {});
});
```
States can contain child states, forming a hierarchy.

## Typical Workflow
- Create a C++ class that makes the machine in an Actor, Component, or Widget
- Build the machine using Harkc's Builder API
- Call `Start()` on `BeginPlay` (or the equivalant)
- Call `Send(...)` in response to a gameplay event

