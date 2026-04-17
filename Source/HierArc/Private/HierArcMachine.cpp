#include "HierArcMachine.h"

void UHierArcMachine::Configure(FHierArcConfig InConfig)
{
    Config = MoveTemp(InConfig);
}

void UHierArcMachine::Start()
{
    if (bRunning) return;
    bRunning = true;

    Context.Send = [this](FName EventName) { Send(EventName); };

    TSharedPtr<FHierArcState> Initial = ResolveState(Config.InitialState);
    if (!Initial.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("HierArc: Initial state '%s' not found."), *Config.InitialState.ToString());
        return;
    }

    EnterState(Config.InitialState, *Initial);
    ActiveStateStack.Add(Config.InitialState);

    FHierArcState* Current = Initial.Get();
    while (Current->IsCompound())
    {
        TSharedPtr<FHierArcState>* Child = Current->States.Find(Current->InitialChildState);
        if (!Child || !Child->IsValid())
        {
            UE_LOG(LogTemp, Error, TEXT("HierArc: Compound state missing initial child '%s'."), *Current->InitialChildState.ToString());
            break;
        }
        EnterState(Current->InitialChildState, **Child);
        ActiveStateStack.Add(Current->InitialChildState);
        Current = Child->Get();
    }
}

void UHierArcMachine::Send(FName EventName)
{
    if (!bRunning) return;

    for (int32 i = ActiveStateStack.Num() - 1; i >= 0; i--)
    {
        TSharedPtr<FHierArcState> State = ResolveState(ActiveStateStack[i]);
        if (!State.IsValid()) continue;

        FName* TargetName = State->Events.Find(EventName);
        if (!TargetName) continue;

        // Exit all active states from leaf down to the handler
        for (int32 j = ActiveStateStack.Num() - 1; j >= i; j--)
        {
            TSharedPtr<FHierArcState> Exiting = ResolveState(ActiveStateStack[j]);
            if (Exiting.IsValid()) ExitState(ActiveStateStack[j], *Exiting);
        }
        ActiveStateStack.SetNum(i);

        // Enter target, drilling into compound children
        TSharedPtr<FHierArcState> Target = ResolveState(*TargetName);
        if (Target.IsValid())
        {
            EnterState(*TargetName, *Target);
            ActiveStateStack.Add(*TargetName);

            FHierArcState* Current = Target.Get();
            while (Current->IsCompound())
            {
                TSharedPtr<FHierArcState>* Child = Current->States.Find(Current->InitialChildState);
                if (!Child || !Child->IsValid()) break;
                EnterState(Current->InitialChildState, **Child);
                ActiveStateStack.Add(Current->InitialChildState);
                Current = Child->Get();
            }
        }
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("HierArc: No handler for event '%s' in current state stack."), *EventName.ToString());
}

FName UHierArcMachine::GetCurrentState() const
{
    if (ActiveStateStack.Num() > 0)
        return ActiveStateStack.Last();
    return NAME_None;
}

void UHierArcMachine::EnterState(FName StateName, FHierArcState& State)
{
    UE_LOG(LogTemp, Log, TEXT("HierArc: Entering '%s'"), *StateName.ToString());
    if (State.OnEntry) State.OnEntry(Context);
}

void UHierArcMachine::ExitState(FName StateName, FHierArcState& State)
{
    UE_LOG(LogTemp, Log, TEXT("HierArc: Exiting '%s'"), *StateName.ToString());
    if (State.OnExit) State.OnExit(Context);
}

TSharedPtr<FHierArcState> UHierArcMachine::ResolveState(FName StateName)
{
    return ResolveStateInMap(StateName, Config.States);
}

TSharedPtr<FHierArcState> UHierArcMachine::ResolveStateInMap(FName StateName, TMap<FName, TSharedPtr<FHierArcState>>& StateMap)
{
    if (TSharedPtr<FHierArcState>* Found = StateMap.Find(StateName))
        return *Found;

    for (auto& Pair : StateMap)
    {
        if (Pair.Value.IsValid())
        {
            TSharedPtr<FHierArcState> Found = ResolveStateInMap(StateName, Pair.Value->States);
            if (Found.IsValid()) return Found;
        }
    }

    return nullptr;
}