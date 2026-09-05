// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "KernelLocalUISubsystem.generated.h"

/**
 * Only for local player. (Widget stack, Rayer)
 * Use with CommonUI
 */
UCLASS()
class KERNEL_API UKernelLocalUISubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()
	
public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
};
