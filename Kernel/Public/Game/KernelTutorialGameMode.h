// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "KernelTutorialGameMode.generated.h"

class AKernelEnemyCharacter;
class AKernelSpawnPoint;
/**
 * 
 */
UCLASS()
class KERNEL_API AKernelTutorialGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditInstanceOnly)
	TArray<TObjectPtr<AKernelSpawnPoint>> SpawnPoint;
	
protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintImplementableEvent)
	void EndTutorial();
};
