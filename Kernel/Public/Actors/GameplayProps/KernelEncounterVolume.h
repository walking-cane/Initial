// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "KernelEncounterVolume.generated.h"

class AKernelRewardCrate;
class AKernelBlockEntrance;
class AKernelEnemyCharacter;
class UBoxComponent;
class AKernelSpawnPoint;
/**
 * 
 */
USTRUCT()
struct FKernelWaveEntry
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag SpawnTag;
	
	UPROPERTY(EditDefaultsOnly)
	int32 SpawnCount = 1;
};

USTRUCT(BlueprintType)
struct FKernelWaveDefinition
{
	GENERATED_BODY()
	
	/** 에디터 표시용, "1-1", "보스" */
	UPROPERTY(EditDefaultsOnly)
	FName DisplayName;
	
	UPROPERTY(EditDefaultsOnly)
	TArray<FKernelWaveEntry> Entries;
	
	UPROPERTY(EditDefaultsOnly)
	float Delay = 2.f;
};

UCLASS()
class KERNEL_API AKernelEncounterVolume : public AActor
{
	GENERATED_BODY()
	
public:
	AKernelEncounterVolume();
	
	void FinishEncounter();
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditInstanceOnly)
	TObjectPtr<AKernelBlockEntrance> BlockEntrance;
	UPROPERTY(EditInstanceOnly)
	TObjectPtr<AKernelRewardCrate> RewardCrate;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> TriggerBox;
	
	UPROPERTY(EditInstanceOnly)
	TArray<TObjectPtr<AKernelSpawnPoint>> SpawnPoints;
	
	UPROPERTY(EditDefaultsOnly)
	TArray<FKernelWaveDefinition> Waves;
	
	UFUNCTION()
	void OnOverlap(
		UPrimitiveComponent* OverlappedComponent, 
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, 
		bool bFromSweep,
		const FHitResult& SweepResult);
	
private:
	TArray<TObjectPtr<AKernelEnemyCharacter>> AliveEnemies;
	
	void CheckWaveCleared();
	void StartWave(int32 WaveIndex);
	
	UFUNCTION() void HandleEnemyDeath(AActor* DeadEnemy);
	
	void RegisterEnemies(TArray<AKernelEnemyCharacter*>& Enemies);
	
	bool bIsWaveActive = false;
	int32 CurrentWaveIndex = INDEX_NONE;
	FTimerHandle WaveTimerHandle;
};
