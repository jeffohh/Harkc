#pragma once
#include "CoreMinimal.h"

struct FHierArcContext;

// No HIERARC_API — struct is header-only, inlined by includers
struct FHierArcState
{
    FName InitialChildState;

    TFunction<void(FHierArcContext&)> OnEntry;
    TFunction<void(FHierArcContext&)> OnExit;

    TMap<FName, FName> Events;
    TMap<FName, TSharedPtr<FHierArcState>> States;

    bool IsCompound() const { return States.Num() > 0; }
};

struct FHierArcContext
{
    TFunction<void(FName)> Send;
};

struct FHierArcConfig
{
    FName Id;
    FName InitialState;
    TMap<FName, TSharedPtr<FHierArcState>> States;
};