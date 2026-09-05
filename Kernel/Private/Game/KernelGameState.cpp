// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/KernelGameState.h"

#include "Game/KernelMapDefinition.h"
#include "KernelCharacter/KernelPlayerState.h"
#include "Net/UnrealNetwork.h"

void AKernelGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AKernelGameState, GamePhase);
	DOREPLIFETIME(AKernelGameState, HostSelectedMapIndex);
}

void AKernelGameState::SetGamePhase(EKernelGamePhase NewPhase)
{
	if (!HasAuthority() || GamePhase == NewPhase) return;
	GamePhase = NewPhase;
	OnRep_GamePhase();
	UE_LOG(LogTemp,Warning,TEXT("Phase Changed to %hhd"), NewPhase);
}

void AKernelGameState::OnRep_GamePhase()
{
	OnGamePhaseChanged.Broadcast(GamePhase);
}

void AKernelGameState::SetHostSelectedMap(int32 MapIndex)
{
	if (!HasAuthority()) return;
	if (!MapRoster.IsValidIndex(MapIndex)) return;

	const UKernelMapDefinition* Def = MapRoster[MapIndex];
	if (!Def || Def->bLocked) return;

	if (Def->LevelPool.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("[GS] Theme '%s' has empty LevelPool — selection rejected"),
			*Def->DisplayName.ToString());
		return;
	}

	HostSelectedMapIndex = MapIndex;
	UE_LOG(LogTemp, Log, TEXT("[GS] Selected theme %d (%s)"), MapIndex, *Def->DisplayName.ToString());

	OnRep_HostSelectedMap();
}

void AKernelGameState::OnRep_HostSelectedMap()
{
	OnMapSelectionChanged.Broadcast();
}

TSoftObjectPtr<UWorld> AKernelGameState::RollLevelFromSelectedTheme() const
{
	if (!HasAuthority()) return nullptr;
	if (!MapRoster.IsValidIndex(HostSelectedMapIndex)) return nullptr;

	const UKernelMapDefinition* Def = MapRoster[HostSelectedMapIndex];
	if (!Def) return nullptr;

	TArray<TSoftObjectPtr<UWorld>> Candidates;
	Candidates.Reserve(Def->LevelPool.Num());
	for (const TSoftObjectPtr<UWorld>& Level : Def->LevelPool)
	{
		if (!Level.IsNull())
		{
			Candidates.Add(Level);
		}
	}

	if (Candidates.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("[GS] Theme %s has no valid level in LevelPool"),
			*Def->DisplayName.ToString());
		return nullptr;
	}

	const TSoftObjectPtr<UWorld> Rolled = Candidates[FMath::RandRange(0, Candidates.Num() - 1)];
	UE_LOG(LogTemp, Log, TEXT("[GS] Rolled level %s from theme %s"),
		*Rolled.ToSoftObjectPath().GetAssetName(), *Def->DisplayName.ToString());

	return Rolled;
}

void AKernelGameState::Kernel_TotalDamageDealt()
{
	if (PlayerArray.IsEmpty()) return;
	
	for (APlayerState* PS : PlayerArray)
	{
		AKernelPlayerState* KPS = Cast<AKernelPlayerState>(PS);
		if (KPS)
		{
			UE_LOG(LogTemp,Log,TEXT("%s Dealt : %f"), *GetNameSafe(KPS), KPS->GetTotalDamageDealt());
		}
	}
}

void AKernelGameState::Multicast_MapPing_Implementation(const FString& PlayerName, int32 MapIndex)
{
	OnMapPing.Broadcast(PlayerName, MapIndex);
}