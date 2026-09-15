// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/KernelGameModeBase.h"

#include "Artifact/KernelArtifactComponent.h"
#include "Artifact/KernelArtifactDefinition.h"
#include "Artifact/KernelArtifactPool.h"
#include "Artifact/KernelArtifactTypes.h"
#include "Game/KernelGameState.h"
#include "Game/KernelMapDefinition.h"
#include "KernelCharacter/KernelPlayerController.h"
#include "KernelCharacter/KernelPlayerState.h"
#include "KernelCharacter/Hero/KernelCharacterDefinition.h"

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

void AKernelGameModeBase::OfferArtifactsToAll(int32 ChoiceCount)
{
    AGameStateBase* GS = GameState;
    if (!GS) return;

    for (APlayerState* PS : GS->PlayerArray)
    {
        OfferArtifactsToPlayer(Cast<AKernelPlayerState>(PS), ChoiceCount);
    }
}

void AKernelGameModeBase::OfferArtifactsToPlayer(AKernelPlayerState* PS, int32 ChoiceCount)
{
    if (!PS || ChoiceCount <= 0)
    {
	    UE_LOG(LogTemp,Error,TEXT("[KGM] No PS? ChoiceCount : %i"), ChoiceCount);
    	return;
    }

    AKernelPlayerController* PC = Cast<AKernelPlayerController>(PS->GetPlayerController());
    if (!PC)
    {
    	UE_LOG(LogTemp,Error,TEXT("[KGM] No Controller!"));
	    return;
    }

    // 1) 공용 + 캐릭터 전용 풀 수집
    TArray<FKernelArtifactPoolEntry> Candidates;
    TSet<const UKernelArtifactPool*> Visited;

    GatherPool(GlobalPool, Candidates, Visited);

    if (const UKernelCharacterDefinition* CharDef = PS->GetCharacterDefinition())
    {
        GatherPool(CharDef->ArtifactPool, Candidates, Visited);
    	UE_LOG(LogTemp,Log,TEXT("[KGM] Gathered CharDef Pools"));
    }

    // 2) 이 플레이어가 이미 가진 고유 아티팩트 제외
    UKernelArtifactComponent* Comp = PS->FindComponentByClass<UKernelArtifactComponent>();
    if (Comp)
    {
        Candidates.RemoveAll([Comp](const FKernelArtifactPoolEntry& E)
        {
            return E.Artifact && E.Artifact->bUnique && Comp->HasArtifact(E.Artifact);
        });
    }

    // 3) 독립 롤
    TArray<UKernelArtifactDefinition*> Picked = PickWeighted(Candidates, ChoiceCount);
    if (Picked.Num() == 0)
    {
    	UE_LOG(LogTemp,Error,TEXT("[KGM] Picked is 0"));
	    return;
    }

    // 4) 제안 구성
    FKernelArtifactOffer Offer;
    Offer.OfferId = NextOfferId++;

    for (int32 i = 0; i < Picked.Num(); ++i)
    {
        FKernelArtifactChoice Choice;
        Choice.ChoiceId = i;
        Choice.Artifact = Picked[i];
        Offer.Choices.Add(Choice);
    }

    PC->OfferArtifacts(Offer);
	UE_LOG(LogTemp,Log,TEXT("[KGM] Offered!"));
}

void AKernelGameModeBase::GatherPool(const UKernelArtifactPool* Pool,
                                 TArray<FKernelArtifactPoolEntry>& Out,
                                 TSet<const UKernelArtifactPool*>& Visited) const
{
    if (!Pool)
    {
    	UE_LOG(LogTemp,Error,TEXT("No Pool"))
	    return;
    }
	
    if (Visited.Contains(Pool))
    {
    	UE_LOG(LogTemp,Error,TEXT("Visited Contains Pool"))
    	return;
    } // 순환 참조 방지
	
    Visited.Add(Pool);

    for (const FKernelArtifactPoolEntry& Entry : Pool->Entries)
    {
        if (!Entry.Artifact || Entry.Weight <= 0.f) continue;

        // 같은 아티팩트가 두 풀에 있으면 가중치를 합산한다
        FKernelArtifactPoolEntry* Existing = Out.FindByPredicate(
            [&Entry](const FKernelArtifactPoolEntry& E)
            {
	            return E.Artifact == Entry.Artifact;
            });

        if (Existing)
        {
	        Existing->Weight += Entry.Weight;
        }
        else
        {
	        Out.Add(Entry);
        	UE_LOG(LogTemp,Log,TEXT("Entry Added"));
        }
    }

    for (const UKernelArtifactPool* Sub : Pool->IncludedPools)
    {
        GatherPool(Sub, Out, Visited);
    }
}

TArray<UKernelArtifactDefinition*> AKernelGameModeBase::PickWeighted(
    TArray<FKernelArtifactPoolEntry>& Candidates, int32 Count) const
{
    TArray<UKernelArtifactDefinition*> Result;

    const int32 Draws = FMath::Min(Count, Candidates.Num());
	UE_LOG(LogTemp,Log,TEXT("[KGM] Picked Weighted! [Count : %i], [Candidates : %i], Draws : %i"), 
		Count, Candidates.Num(), Draws);
	
    for (int32 Draw = 0; Draw < Draws; ++Draw)
    {
        float Total = 0.f;
        for (const FKernelArtifactPoolEntry& E : Candidates) { Total += E.Weight; }
        if (Total <= 0.f) break;

        float Pick = FMath::FRand() * Total;
        int32 Chosen = Candidates.Num() - 1;   // 부동소수 오차 폴백

        for (int32 i = 0; i < Candidates.Num(); ++i)
        {
            Pick -= Candidates[i].Weight;
            if (Pick < 0.f) { Chosen = i; break; }
        }

        Result.Add(Candidates[Chosen].Artifact.Get());
        Candidates.RemoveAt(Chosen);   // 중복 방지 — 같은 게 두 칸에 뜨지 않도록
    }

    return Result;
}