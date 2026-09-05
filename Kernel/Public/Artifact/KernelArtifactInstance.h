// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/KernelItemInstance.h"
#include "KernelArtifactInstance.generated.h"

/**
 * 
 */
UCLASS()
class KERNEL_API UKernelArtifactInstance : public UKernelItemInstance
{
	GENERATED_BODY()
	
public:
	UPROPERTY()	int32 AbilityStacks;
};
