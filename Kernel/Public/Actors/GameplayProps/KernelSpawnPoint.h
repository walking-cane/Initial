// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "KernelSpawnPoint.generated.h"

class AKernelEnemyCharacter;
class UKernelEnemyDefinition;

UCLASS(BlueprintType)
class KERNEL_API AKernelSpawnPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	AKernelSpawnPoint();
	
	void SpawnEnemies(int32 Counter, const FVector& ShowPoint, TArray<AKernelEnemyCharacter*>& OutArray);
	
	UPROPERTY(EditInstanceOnly, Category = "Spawn")
	FGameplayTag SpawnTag;

protected:
	
	UPROPERTY(EditInstanceOnly, Category = "Spawn")
	TObjectPtr<UKernelEnemyDefinition> EnemyDef;
	
	UPROPERTY(EditInstanceOnly, Category="Spawn", meta=(ClampMin="0", ClampMax="180"))
	float SpawnConeAngle = 60.f;
	
	UPROPERTY(EditInstanceOnly, Category="Spawn")
	FFloatInterval SpawnOffset = FFloatInterval(150.f, 300.f);
};
