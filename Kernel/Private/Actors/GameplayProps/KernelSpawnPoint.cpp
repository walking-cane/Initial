// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/GameplayProps/KernelSpawnPoint.h"

#include "NavigationSystem.h"
#include "KernelCharacter/Enemy/KernelEnemyCharacter.h"
#include "KernelCharacter/Enemy/KernelEnemyDefinition.h"

// Sets default values
AKernelSpawnPoint::AKernelSpawnPoint()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AKernelSpawnPoint::SpawnEnemies(int32 Counter, const FVector& ShowPoint, TArray<AKernelEnemyCharacter*>& OutArray)
{
	if (!HasAuthority()) return;
	if (!EnemyDef)
	{
		UE_LOG(LogTemp,Error,TEXT("[SpawnPoint] EnemyDefinition is not set"))
		return;
	}
	
	for (int32 i = 0; i < Counter; ++i)
	{
		const FVector Dir = FMath::VRandCone(
			GetActorForwardVector(),
			FMath::DegreesToRadians(SpawnConeAngle * 0.5f),
			0.f); 
		FVector SpawnLoc = GetActorLocation() + Dir * FMath::FRandRange(SpawnOffset.Min, SpawnOffset.Max);
		
		if (const UNavigationSystemV1* Nav = UNavigationSystemV1::GetCurrent(GetWorld()))
		{
			FNavLocation NavLoc;
			if (Nav->ProjectPointToNavigation(SpawnLoc, NavLoc, FVector(200.f, 200.f, 300.f)))
			{
				SpawnLoc = NavLoc.Location;
			}
		}
		const FRotator SpawnRot = (ShowPoint - SpawnLoc).GetSafeNormal2D().Rotation();
		FTransform SpawnTransform(SpawnRot, SpawnLoc);
		
		DrawDebugSphere(GetWorld(), SpawnLoc, 32.f, 32, FColor::Cyan, false, 100.f);
		AKernelEnemyCharacter* Enemy = GetWorld()->SpawnActorDeferred<AKernelEnemyCharacter>(
			EnemyDef->EnemyClass, 
			SpawnTransform,
			nullptr,nullptr);
		
		if (Enemy)
		{
			Enemy->FinishSpawning(SpawnTransform);
			OutArray.Add(Enemy);
		}
	}
}