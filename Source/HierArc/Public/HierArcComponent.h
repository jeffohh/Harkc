#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HierArcMachine.h"
#include "HierArcComponent.generated.h"

UCLASS(Abstract, ClassGroup="HierArc", meta=(BlueprintSpawnableComponent))
class HIERARC_API UHierArcComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UHierArcComponent();

    UFUNCTION(BlueprintCallable, Category="HierArc")
    void Send(FName EventName);

    UFUNCTION(BlueprintCallable, Category="HierArc")
    FName GetCurrentState() const;

    UHierArcMachine* GetMachine() const { return Machine; }

protected:
    virtual void BeginPlay() override;

    // Override this in your subclass to build and return the machine
    virtual UHierArcMachine* BuildMachine() PURE_VIRTUAL(UHierArcComponent::BuildMachine, return nullptr;);

private:
    UPROPERTY()
    UHierArcMachine* Machine = nullptr;
};