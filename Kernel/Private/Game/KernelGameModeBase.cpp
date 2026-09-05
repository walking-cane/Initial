// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/KernelGameModeBase.h"
#include "Game/KernelGameState.h"
#include "Game/KernelMapDefinition.h"
#include "KernelCharacter/KernelPlayerState.h"

AKernelGameModeBase::AKernelGameModeBase()
{
	// 허브 → 게임 맵 트래블에서 PlayerState(캐릭터 픽)를 보존하기 위해 필수
	bUseSeamlessTravel = true;
}

void AKernelGameModeBase::InitGameState()
{
	Super::InitGameState();
	if (AKernelGameState* GS = GetGameState<AKernelGameState>())
	{
		GS->SetGamePhase(InitialPhase);
	}
}

void AKernelGameModeBase::ConfirmMapAndBeginCharacterSelect()
{
	AKernelGameState* GS = GetGameState<AKernelGameState>();
	if (!GS || GS->GetGamePhase() != EKernelGamePhase::MapSelect) return;
	if (!GS->GetMapRoster().IsValidIndex(GS->GetHostSelectedMapIndex())) return;

	GS->SetGamePhase(EKernelGamePhase::CharacterSelect);
}

void AKernelGameModeBase::BeginDeparture()
{
	AKernelGameState* GS = GetGameState<AKernelGameState>();
	if (!GS || GS->GetGamePhase() != EKernelGamePhase::CharacterSelect) return;

	// 전원 픽 완료 검증
	for (APlayerState* PS : GS->PlayerArray)
	{
		const AKernelPlayerState* KPS = Cast<AKernelPlayerState>(PS);
		if (!KPS || KPS->GetSelectedCharacterIndex() == INDEX_NONE)
		{
			UE_LOG(LogTemp, Warning, TEXT("[GM] 출발 거부 — %s 미선택"), *GetNameSafe(PS));
			return;
		}
	}

	// 목적지를 먼저 확정한다. 실패하면 Departing으로 넘어가기 전에 중단.
	const TSoftObjectPtr<UWorld> Level = GS->RollLevelFromSelectedTheme();
	if (Level.IsNull())
	{
		UE_LOG(LogTemp, Error, TEXT("[GM] 출발 거부 — 레벨 추첨 실패"));
		return;
	}

	GS->SetGamePhase(EKernelGamePhase::Departing);

	// 카메라 페이드 길이만큼 대기 후 트래블
	TWeakObjectPtr<AKernelGameModeBase> WeakThis(this);
	FTimerHandle DepartureHandle;
	GetWorldTimerManager().SetTimer(DepartureHandle, [WeakThis, Level]()
	{
		if (AKernelGameModeBase* GM = WeakThis.Get())
		{
			GM->GetWorld()->ServerTravel(Level.ToSoftObjectPath().GetLongPackageName());
		}
	},1.f + 0.3f, false);

	UE_LOG(LogLevelStreaming, Warning, TEXT("Travel start to %s"),
		*Level.ToSoftObjectPath().GetLongPackageName());
}