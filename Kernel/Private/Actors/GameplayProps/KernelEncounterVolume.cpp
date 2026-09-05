// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/GameplayProps/KernelEncounterVolume.h"
#include "Actors/GameplayProps/KernelBlockEntrance.h"
#include "Actors/GameplayProps/KernelRewardCrate.h"
#include "Actors/GameplayProps/KernelSpawnPoint.h"
#include "Components/BoxComponent.h"
#include "KernelCharacter/KernelHealthComponent.h"
#include "KernelCharacter/KernelPlayerState.h"
#include "KernelCharacter/Enemy/KernelEnemyCharacter.h"

AKernelEncounterVolume::AKernelEncounterVolume()
{
	PrimaryActorTick.bCanEverTick = false;
	
	TriggerBox = CreateDefaultSubobject<UBoxComponent>("TriggerBox");
	TriggerBox->SetCollisionProfileName("Trigger");
	SetRootComponent(TriggerBox);
}

void AKernelEncounterVolume::BeginPlay()
{
	Super::BeginPlay();
	
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AKernelEncounterVolume::OnOverlap);
}

void AKernelEncounterVolume::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                     UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority() || bIsWaveActive) return;
	
	if (SpawnPoints.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("[EncounterVolume] SpawnPoints is empty!"));
		return;
	}
	
	const APawn* Pawn = Cast<APawn>(OtherActor);
	if (!Pawn) return;
	
	if (const AKernelPlayerState* KPS = Pawn->GetPlayerState<AKernelPlayerState>())
	{
		StartWave(0);
		SetActorEnableCollision(false);
	}
}

void AKernelEncounterVolume::FinishEncounter()
{
	UE_LOG(LogTemp, Log, TEXT("[EncounterVolume] Wave Cleared!"));
	BlockEntrance->OpenDoor();
	RewardCrate->OnWaveCleared();
}

void AKernelEncounterVolume::CheckWaveCleared()
{
	if (!bIsWaveActive) return;
	AliveEnemies.RemoveAll([](AKernelEnemyCharacter* E) { return !IsValid(E); });
	
	if (!AliveEnemies.IsEmpty()) return;
	
	UE_LOG(LogTemp, Log, TEXT("[EncounterVolume] CheckingEnemy"));
	
	bIsWaveActive = false;
	
	const int32 NextWaveIndex = CurrentWaveIndex + 1;
	if (!Waves.IsValidIndex(NextWaveIndex))
	{
		FinishEncounter();
		return;
	}
	
	GetWorldTimerManager().SetTimer(WaveTimerHandle,
		[this, NextWaveIndex]() { StartWave(NextWaveIndex); },
		Waves[NextWaveIndex].Delay, false);
}

void AKernelEncounterVolume::StartWave(int32 WaveIndex)
{
	if (!Waves.IsValidIndex(WaveIndex)) return;
	
	CurrentWaveIndex = WaveIndex;
	const FKernelWaveDefinition& Wave = Waves[WaveIndex];
	
	for (const FKernelWaveEntry& Entry  : Wave.Entries)
	{
		TArray<AKernelSpawnPoint*> MatchedPoints;
		for (AKernelSpawnPoint* SpawnPoint : SpawnPoints)
		{
			if (SpawnPoint && SpawnPoint->SpawnTag == Entry.SpawnTag)
			{
				MatchedPoints.Add(SpawnPoint);
			}
		}
		
		if (MatchedPoints.IsEmpty())
		{
			UE_LOG(LogTemp,Error,TEXT("[EncounterVolume] No matched points"));
			continue;
		}
		
		// 라운드로빈으로 개수 분배 (5마리 / 2곳 = 3, 2)
		for (int32 i = 0; i < MatchedPoints.Num(); i++)
		{
			const int32 Share = Entry.SpawnCount / MatchedPoints.Num()
							  + (i < Entry.SpawnCount % MatchedPoints.Num() ? 1 : 0);
			if (Share <= 0) continue;

			TArray<AKernelEnemyCharacter*> Spawned;
			MatchedPoints[i]->SpawnEnemies(Share, MatchedPoints[i]->GetActorLocation(), Spawned);
			RegisterEnemies(Spawned);
		}
	}
	
	bIsWaveActive = true;
	UE_LOG(LogTemp,Warning,TEXT("[Encounter] Wave Start {%d}"), CurrentWaveIndex);
}

void AKernelEncounterVolume::HandleEnemyDeath(AActor* DeadEnemy)
{
	const int32 Removed = AliveEnemies.Remove(Cast<AKernelEnemyCharacter>(DeadEnemy));

	UE_LOG(LogTemp, Warning, TEXT("[Encounter] Death: %s | Removed=%d | Remaining=%d"),
		*GetNameSafe(DeadEnemy), Removed, AliveEnemies.Num());

	CheckWaveCleared();
}

void AKernelEncounterVolume::RegisterEnemies(TArray<AKernelEnemyCharacter*>& Enemies)
{
	for (AKernelEnemyCharacter* Enemy : Enemies)
	{
		UE_LOG(LogTemp,Warning,TEXT("[EncounterVolume] Spawned : %s"), *GetNameSafe(Enemy));
		if (!Enemy) continue;

		UKernelHealthComponent* HC = Enemy->FindComponentByClass<UKernelHealthComponent>();
		if (!HC) continue;
		
		AliveEnemies.AddUnique(Enemy);
		HC->OnDeathStarted.AddDynamic(this, &ThisClass::HandleEnemyDeath);
	}
}

