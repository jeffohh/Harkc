#pragma once
#include "CoreMinimal.h"
#include "HierArcTypes.h"

// Passed into the lambda for each State() call
class HIERARC_API FHierArcStateBuilder
{
public:
    FHierArcStateBuilder(FHierArcState& InState) : State(InState) {}

    // Set OnEntry callback
    FHierArcStateBuilder& OnEntry(TFunction<void(FHierArcContext&)> Callback)
    {
        State.OnEntry = MoveTemp(Callback);
        return *this;
    }

    // Set OnExit callback
    FHierArcStateBuilder& OnExit(TFunction<void(FHierArcContext&)> Callback)
    {
        State.OnExit = MoveTemp(Callback);
        return *this;
    }

    // Register an event -> target transition
    FHierArcStateBuilder& On(FName EventName, FName TargetState)
    {
        State.Events.Add(EventName, TargetState);
        return *this;
    }

    // Define a nested child state (for compound states)
    FHierArcStateBuilder& State(FName StateName, TFunction<void(FHierArcStateBuilder&)> BuildFn)
    {
        TSharedPtr<FHierArcState> Child = MakeShared<FHierArcState>();
        FHierArcStateBuilder ChildBuilder(*Child);
        BuildFn(ChildBuilder);
        this->State.States.Add(StateName, Child);
        return *this;
    }

    // Set the initial child state (for compound states)
    FHierArcStateBuilder& Initial(FName StateName)
    {
        State.InitialChildState = StateName;
        return *this;
    }

private:
    FHierArcState& State;
};