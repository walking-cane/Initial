// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "KernelEnemyDefinition.generated.h"

class AKernelEnemyCharacter;

UCLASS()
class KERNEL_API UKernelEnemyDefinition : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<AKernelEnemyCharacter> EnemyClass;
};
