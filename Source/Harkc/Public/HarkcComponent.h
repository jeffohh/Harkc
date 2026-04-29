#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HarkcMachine.h"
#include "HarkcComponent.generated.h"

UCLASS(Abstract, ClassGroup="Harkc", meta=(BlueprintSpawnableComponent))
class HARKC_API UHarkcComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UHarkcComponent();

    UFUNCTION(BlueprintCallable, Category="Harkc")
    void Send(FName EventName);

    UFUNCTION(BlueprintCallable, Category="Harkc")
    FName GetCurrentState() const;

    UHarkcMachine* GetMachine() const { return Machine; }

protected:
    virtual void BeginPlay() override;

    // Override this in your subclass to build and return the machine
    virtual UHarkcMachine* BuildMachine() PURE_VIRTUAL(UHarkcComponent::BuildMachine, return nullptr;);

private:
    UPROPERTY()
    UHarkcMachine* Machine = nullptr;
};