#pragma once
#include "CoreMinimal.h"

struct FHarkcContext;

struct FHarkcTransition
{
    FName TargetState;
    TArray<TFunction<bool(FHarkcContext&)>> Guards;
    TArray<TFunction<void(FHarkcContext&)>> Actions;
};

struct FHarkcState
{
    FName InitialChildState;

    TFunction<void(FHarkcContext&)> OnEntry;
    TFunction<void(FHarkcContext&)> OnExit;

    TMap<FName, FHarkcTransition> Events;
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