// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "KernelGlobalUISubsystem.generated.h"

/**
 * Global UI subsystem.
 * Local UI is in ULocalPlayerUISubsystem.
 */
UCLASS()
class KERNEL_API UKernelGlobalUISubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
};
