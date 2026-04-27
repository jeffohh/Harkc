#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "HierArcTypes.h"
#include "HierArcBuilder.h"
#include "HierArcMachine.generated.h"

UCLASS(BlueprintType, Blueprintable)
class HIERARC_API UHierArcMachine : public UObject
{
    GENERATED_BODY()

public:
    // Entry point — replaces NewObject + Configure
    static UHierArcMachine* Create(UObject* Outer, FName MachineId);

    // Builder methods — each returns 'this' for chaining
    UHierArcMachine* Initial(FName StateName);
    UHierArcMachine* State(FName StateName, TFunction<void(FHierArcStateBuilder&)> BuildFn);

    // Starts the machine and returns itself so you can chain Start() at the end
    UHierArcMachine* Start();

    void Send(FName EventName);

    UFUNCTION(BlueprintCallable)
    FName GetCurrentState() const;

    bool IsRunning() const { return bRunning; }

private:
    FHierArcConfig Config;
    FHierArcContext Context;
    TArray<FName> ActiveStateStack;
    bool bRunning = false;

    void EnterState(FName StateName, FHierArcState& State);
    void ExitState(FName StateName, FHierArcState& State);

    TSharedPtr<FHierArcState> ResolveState(FName StateName);
    TSharedPtr<FHierArcState> ResolveStateInMap(FName StateName, TMap<FName, TSharedPtr<FHierArcState>>& StateMap);
};