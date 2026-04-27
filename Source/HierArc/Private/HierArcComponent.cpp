#include "HierArcComponent.h"

UHierArcComponent::UHierArcComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UHierArcComponent::BeginPlay()
{
    Super::BeginPlay();

    // Build and start the machine defined by the subclass
    Machine = BuildMachine();

    if (!Machine)
        UE_LOG(LogTemp, Error, TEXT("HierArc: %s did not return a machine from BuildMachine()."), *GetName());
}

void UHierArcComponent::Send(FName EventName)
{
    if (Machine)
        Machine->Send(EventName);
    else
        UE_LOG(LogTemp, Warning, TEXT("HierArc: Send('%s') called but no machine is set."), *EventName.ToString());
}

FName UHierArcComponent::GetCurrentState() const
{
    if (Machine)
        return Machine->GetCurrentState();
    return NAME_None;
}