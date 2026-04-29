#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "HarkcTypes.h"
#include "HarkcBuilder.h"
#include "HarkcMachine.generated.h"

UCLASS(BlueprintType, Blueprintable)
class HARKC_API UHarkcMachine : public UObject
{
    GENERATED_BODY()

public:
    // Entry point — replaces NewObject + Configure
    static UHarkcMachine* Create(UObject* Outer, FName MachineId);

    // Builder methods — each returns 'this' for chaining
    UHarkcMachine* Initial(FName StateName);
    UHarkcMachine* State(FName StateName, TFunction<void(FHarkcStateBuilder&)> BuildFn);

    // Starts the machine and returns itself so you can chain Start() at the end
    UHarkcMachine* Start();

    void Send(FName EventName);

    UFUNCTION(BlueprintCallable)
    FName GetCurrentState() const;

    bool IsRunning() const { return bRunning; }

private:
    FHarkcConfig Config;
    FHarkcContext Context;
    TArray<FName> ActiveStateStack;
    bool bRunning = false;

    void EnterState(FName StateName, FHarkcState& State);
    void ExitState(FName StateName, FHarkcState& State);

    TSharedPtr<FHarkcState> ResolveState(FName StateName);
    TSharedPtr<FHarkcState> ResolveStateInMap(FName StateName, TMap<FName, TSharedPtr<FHarkcState>>& StateMap);
};