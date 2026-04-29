#include "HarkcMachine.h"

UHarkcMachine* UHarkcMachine::Create(UObject* Outer, FName MachineId)
{
    UHarkcMachine* Machine = NewObject<UHarkcMachine>(Outer);
    Machine->Config.Id = MachineId;
    return Machine;
}

UHarkcMachine* UHarkcMachine::Initial(FName StateName)
{
    Config.InitialState = StateName;
    return this;
}

UHarkcMachine* UHarkcMachine::State(FName StateName, TFunction<void(FHarkcStateBuilder&)> BuildFn)
{
    TSharedPtr<FHarkcState> NewState = MakeShared<FHarkcState>();
    FHarkcStateBuilder Builder(*NewState);
    BuildFn(Builder);
    Config.States.Add(StateName, NewState);
    return this;
}

UHarkcMachine* UHarkcMachine::Start()
{
    if (bRunning) return this;
    bRunning = true;

    Context.Send = [this](FName EventName){ Send(EventName); };

    TSharedPtr<FHarkcState> Initial = ResolveState(Config.InitialState);
    if (!Initial.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("Harkc: Initial state '%s' not found."), *Config.InitialState.ToString());
        return this;
    }

    EnterState(Config.InitialState, *Initial);
    ActiveStateStack.Add(Config.InitialState);

    FHarkcState* Current = Initial.Get();
    while (Current->IsCompound())
    {
        TSharedPtr<FHarkcState>* Child = Current->States.Find(Current->InitialChildState);
        if (!Child || !Child->IsValid())
        {
            UE_LOG(LogTemp, Error, TEXT("Harkc: Compound state missing initial child '%s'."), *Current->InitialChildState.ToString());
            break;
        }
        EnterState(Current->InitialChildState, **Child);
        ActiveStateStack.Add(Current->InitialChildState);
        Current = Child->Get();
    }

    return this;
}

void UHarkcMachine::Send(FName EventName)
{
    if (!bRunning) return;

    for (int32 i = ActiveStateStack.Num() - 1; i >= 0; i--)
    {
        TSharedPtr<FHarkcState> State = ResolveState(ActiveStateStack[i]);
        if (!State.IsValid()) continue;

        FName* TargetName = State->Events.Find(EventName);
        if (!TargetName) continue;

        for (int32 j = ActiveStateStack.Num() - 1; j >= i; j--)
        {
            TSharedPtr<FHarkcState> Exiting = ResolveState(ActiveStateStack[j]);
            if (Exiting.IsValid()) ExitState(ActiveStateStack[j], *Exiting);
        }
        ActiveStateStack.SetNum(i);

        TSharedPtr<FHarkcState> Target = ResolveState(*TargetName);
        if (Target.IsValid())
        {
            EnterState(*TargetName, *Target);
            ActiveStateStack.Add(*TargetName);

            FHarkcState* Current = Target.Get();
            while (Current->IsCompound())
            {
                TSharedPtr<FHarkcState>* Child = Current->States.Find(Current->InitialChildState);
                if (!Child || !Child->IsValid()) break;
                EnterState(Current->InitialChildState, **Child);
                ActiveStateStack.Add(Current->InitialChildState);
                Current = Child->Get();
            }
        }
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("Harkc: No handler for event '%s' in current state stack."), *EventName.ToString());
}

FName UHarkcMachine::GetCurrentState() const
{
    if (ActiveStateStack.Num() > 0)
        return ActiveStateStack.Last();
    return NAME_None;
}

void UHarkcMachine::EnterState(FName StateName, FHarkcState& State)
{
    UE_LOG(LogTemp, Log, TEXT("Harkc: Entering '%s'"), *StateName.ToString());
    if (State.OnEntry) State.OnEntry(Context);
}

void UHarkcMachine::ExitState(FName StateName, FHarkcState& State)
{
    UE_LOG(LogTemp, Log, TEXT("Harkc: Exiting '%s'"), *StateName.ToString());
    if (State.OnExit) State.OnExit(Context);
}

TSharedPtr<FHarkcState> UHarkcMachine::ResolveState(FName StateName)
{
    return ResolveStateInMap(StateName, Config.States);
}

TSharedPtr<FHarkcState> UHarkcMachine::ResolveStateInMap(FName StateName, TMap<FName, TSharedPtr<FHarkcState>>& StateMap)
{
    if (TSharedPtr<FHarkcState>* Found = StateMap.Find(StateName))
        return *Found;

    for (auto& Pair : StateMap)
    {
        if (Pair.Value.IsValid())
        {
            TSharedPtr<FHarkcState> Found = ResolveStateInMap(StateName, Pair.Value->States);
            if (Found.IsValid()) return Found;
        }
    }

    return nullptr;
}