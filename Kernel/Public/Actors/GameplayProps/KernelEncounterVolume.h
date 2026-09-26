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
	
	UPROPERTY(EditAnywhere)    
	FGameplayTag SpawnTag;

	UPROPERTY(EditAnywhere)   
	int32 SpawnCount = 1;
};

USTRUCT(BlueprintType)
struct FKernelWaveDefinition
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)  
	FName DisplayName;

	UPROPERTY(EditAnywhere)    
	TArray<FKernelWaveEntry> Entries;

	UPROPERTY(EditAnywhere)    
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
	
	UPROPERTY(EditAnywhere)
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
	UPROPERTY(EditInstanceOnly) //EditInstanceOnly 속성 추가
	TArray<TObjectPtr<AKernelEnemyCharacter>> AliveEnemies;
	
	void CheckWaveCleared();
	void StartWave(int32 WaveIndex);
	
	UFUNCTION() void HandleEnemyDeath(AActor* DeadEnemy);
	
	void RegisterEnemies(TArray<AKernelEnemyCharacter*>& Enemies);
	
	bool bIsWaveActive = false;
	int32 CurrentWaveIndex = INDEX_NONE;
	FTimerHandle WaveTimerHandle;
};
