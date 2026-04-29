#pragma once
#include "CoreMinimal.h"

struct FHarkcContext;

// No HARKC_API – struct is header-only, inlined by includers
struct FHarkcState
{
    FName InitialChildState;

    TFunction<void(FHarkcContext&)> OnEntry;
    TFunction<void(FHarkcContext&)> OnExit;

    TMap<FName, FName> Events;
    TMap<FName, TSharedPtr<FHarkcState>> States;

    bool IsCompound() const { return States.Num() > 0; }
};

struct FHarkcContext
{
    TFunction<void(FName)> Send;
};

struct FHarkcConfig
{
    FName Id;
    FName InitialState;
    TMap<FName, TSharedPtr<FHarkcState>> States;
};