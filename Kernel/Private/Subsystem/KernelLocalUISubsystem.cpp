// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/KernelLocalUISubsystem.h"

bool UKernelLocalUISubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (!Cast<ULocalPlayer>(Outer))
	{
		return false;
	}
	return Super::ShouldCreateSubsystem(Outer);
}

void UKernelLocalUISubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UKernelLocalUISubsystem::Deinitialize()
{
	Super::Deinitialize();
}
