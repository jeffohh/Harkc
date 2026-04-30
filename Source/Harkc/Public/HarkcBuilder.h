#pragma once
#include "CoreMinimal.h"
#include "HarkcTypes.h"

class FHarkcStateBuilder;

// [=== TRANSITION BUILDER ===]
class HARKC_API FHarkcTransitionBuilder
{
    friend class FHarkcStateBuilder;

public:
    FHarkcTransitionBuilder(FHarkcTransition& InTransition, FHarkcStateBuilder& InStateBuilder)
        : TransitionRef(&InTransition), StateBuilder(InStateBuilder) {}

    FHarkcTransitionBuilder& Guard(TFunction<bool(FHarkcContext&)> Fn)
    {
        TransitionRef->Guards.Add(MoveTemp(Fn));
        return *this;
    }

    FHarkcTransitionBuilder& Action(TFunction<void(FHarkcContext&)> Fn)
    {
        TransitionRef->Actions.Add(MoveTemp(Fn));
        return *this;
    }

    // Forwarding declarations only — defined after FHarkcStateBuilder
    FHarkcTransitionBuilder& On(FName EventName, FName TargetState);
    FHarkcTransitionBuilder& OnEntry(TFunction<void(FHarkcContext&)> Callback);
    FHarkcTransitionBuilder& OnExit(TFunction<void(FHarkcContext&)> Callback);
    FHarkcTransitionBuilder& State(FName StateName, TFunction<void(FHarkcStateBuilder&)> BuildFn);
    FHarkcTransitionBuilder& Initial(FName StateName);

private:
    FHarkcTransition* TransitionRef;
    FHarkcStateBuilder& StateBuilder;
};

// [=== STATE BUILDER ===]
class HARKC_API FHarkcStateBuilder
{
    friend class FHarkcTransitionBuilder;

public:
    FHarkcStateBuilder(FHarkcState& InState) : StateRef(InState) {}

    FHarkcStateBuilder& OnEntry(TFunction<void(FHarkcContext&)> Callback)
    {
        StateRef.OnEntry = MoveTemp(Callback);
        return *this;
    }

    FHarkcStateBuilder& OnExit(TFunction<void(FHarkcContext&)> Callback)
    {
        StateRef.OnExit = MoveTemp(Callback);
        return *this;
    }

    FHarkcTransitionBuilder On(FName EventName, FName TargetState)
    {
        FHarkcTransition& T = StateRef.Events.Add(EventName, FHarkcTransition{ TargetState });
        return FHarkcTransitionBuilder(T, *this);
    }

    FHarkcStateBuilder& State(FName StateName, TFunction<void(FHarkcStateBuilder&)> BuildFn)
    {
        TSharedPtr<FHarkcState> Child = MakeShared<FHarkcState>();
        FHarkcStateBuilder ChildBuilder(*Child);
        BuildFn(ChildBuilder);
        StateRef.States.Add(StateName, Child);
        return *this;
    }

    FHarkcStateBuilder& Initial(FName StateName)
    {
        StateRef.InitialChildState = StateName;
        return *this;
    }

private:
    FHarkcState& StateRef;
};


// --- Transition Builder Method Definitions ---
inline FHarkcTransitionBuilder& FHarkcTransitionBuilder::On(FName EventName, FName TargetState)
{
    FHarkcTransition& T = StateBuilder.StateRef.Events.Add(EventName, FHarkcTransition{ TargetState });
    TransitionRef = &T;
    return *this;
}

inline FHarkcTransitionBuilder& FHarkcTransitionBuilder::OnEntry(TFunction<void(FHarkcContext&)> Callback)
{
    StateBuilder.OnEntry(MoveTemp(Callback));
    return *this;
}

inline FHarkcTransitionBuilder& FHarkcTransitionBuilder::OnExit(TFunction<void(FHarkcContext&)> Callback)
{
    StateBuilder.OnExit(MoveTemp(Callback));
    return *this;
}

inline FHarkcTransitionBuilder& FHarkcTransitionBuilder::State(FName StateName, TFunction<void(FHarkcStateBuilder&)> BuildFn)
{
    StateBuilder.State(StateName, MoveTemp(BuildFn));
    return *this;
}

inline FHarkcTransitionBuilder& FHarkcTransitionBuilder::Initial(FName StateName)
{
    StateBuilder.Initial(StateName);
    return *this;
}