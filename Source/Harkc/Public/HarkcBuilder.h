#pragma once
#include "CoreMinimal.h"
#include "HarkcTypes.h"

// Passed into the lambda for each State() call
class HARKC_API FHarkcStateBuilder
{
public:
    FHarkcStateBuilder(FHarkcState& InState) : State(InState) {}

    // Set OnEntry callback
    FHarkcStateBuilder& OnEntry(TFunction<void(FHarkcContext&)> Callback)
    {
        State.OnEntry = MoveTemp(Callback);
        return *this;
    }

    // Set OnExit callback
    FHarkcStateBuilder& OnExit(TFunction<void(FHarkcContext&)> Callback)
    {
        State.OnExit = MoveTemp(Callback);
        return *this;
    }

    // Register an event -> target transition
    FHarkcStateBuilder& On(FName EventName, FName TargetState)
    {
        State.Events.Add(EventName, TargetState);
        return *this;
    }

    // Define a nested child state (for compound states)
    FHarkcStateBuilder& State(FName StateName, TFunction<void(FHarkcStateBuilder&)> BuildFn)
    {
        TSharedPtr<FHarkcState> Child = MakeShared<FHarkcState>();
        FHarkcStateBuilder ChildBuilder(*Child);
        BuildFn(ChildBuilder);
        this->State.States.Add(StateName, Child);
        return *this;
    }

    // Set the initial child state (for compound states)
    FHarkcStateBuilder& Initial(FName StateName)
    {
        State.InitialChildState = StateName;
        return *this;
    }

private:
    FHarkcState& State;
};