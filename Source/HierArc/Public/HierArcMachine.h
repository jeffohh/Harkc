#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "HierArcTypes.h"
#include "HierArcMachine.generated.h"

UCLASS(BlueprintType, Blueprintable)
class HIERARC_API UHierArcMachine : public UObject
{
    GENERATED_BODY()

public:
    // No UFUNCTION — FHierArcConfig is not a USTRUCT, UHT can't reflect it
    void Configure(FHierArcConfig InConfig);

    void Start();
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