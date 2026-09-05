// Fill out your copyright notice in the Description page of Project Settings.


#include "KernelCharacter/KernelPlayerState.h"
#include "AbilitySystemComponent.h"
#include "Game/KernelGameState.h"
#include "Game/KernelMapDefinition.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameplayAbility/KernelAbilitySystemComponent.h"
#include "GameplayAbility/KernelGameplayTags.h"
#include "GameplayAbility/Attributes/KernelCombatAttributeSet.h"
#include "KernelCharacter/Hero/KernelCharacterDefinition.h"
#include "Messages/KernelVerbMessage.h"
#include "Net/UnrealNetwork.h"

void AKernelPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AKernelPlayerState, SelectedCharacterIndex);
	DOREPLIFETIME(AKernelPlayerState, CharacterDefinition);
	DOREPLIFETIME(AKernelPlayerState, VotedMapIndex);
	DOREPLIFETIME(AKernelPlayerState, TotalDamageDealt);
	DOREPLIFETIME(AKernelPlayerState, bIsBoarded);
	DOREPLIFETIME_CONDITION(AKernelPlayerState, UnlockedCharacterIds, COND_OwnerOnly);
}

AKernelPlayerState::AKernelPlayerState()
{
	SetNetUpdateFrequency(100.0f); //AbilitySystemComponent needs to be updated at a high frequency.
	
	KernelASC = CreateDefaultSubobject<UKernelAbilitySystemComponent>(TEXT("ASC"));
	KernelASC->SetIsReplicated(true);
	KernelASC->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	
	HealthSet = CreateDefaultSubobject<UKernelHealthAttributeSet>(TEXT("HealthSet"));
	CombatSet = CreateDefaultSubobject<UKernelCombatAttributeSet>(TEXT("CombatSet"));
}

void AKernelPlayerState::SetCharacterDefinition(UKernelCharacterDefinition* InDef)
{
	check(InDef);
	CharacterDefinition = InDef;
}

void AKernelPlayerState::ServerRPC_ChangeCharacter_Implementation(int32 CharacterIndex)
{
	AKernelGameState* GS = GetWorld()->GetGameState<AKernelGameState>();
	if (!GS) return;

	// 중복 허용이므로 검증은 "로스터 범위 + 로비 페이즈인가"만
	if (!GS->GetCharacterRoster().IsValidIndex(CharacterIndex)) return;
	if (GS->GetGamePhase() != EKernelGamePhase::CharacterSelect) return; // 게임 시작 후 변경 방지
	
	const auto& Roster = GS->GetCharacterRoster();
	if (!Roster.IsValidIndex(CharacterIndex) || !Roster[CharacterIndex]) return;
	if (!IsCharacterUnlocked(Roster[CharacterIndex]->CharacterId))
	{
		UE_LOG(LogTemp, Warning, TEXT("[PS] 미해금 캐릭터 선택 시도: %s"), *GetPlayerName());
		return;
	}

	SelectedCharacterIndex = CharacterIndex;
	CharacterDefinition = Roster[CharacterIndex];
	
	UE_LOG(LogTemp,Warning,TEXT("[PS] Character Changed to %s"), *CharacterDefinition->CharacterId.ToString());

	OnRep_SelectedCharacter();
}

void AKernelPlayerState::OnRep_SelectedCharacter()
{
	OnLobbySelectionChanged.Broadcast();
}

void AKernelPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	// seamless travel 시 새 PlayerState로 픽 정보를 이월 — 이게 없으면 게임 맵에서 픽이 초기화됩니다
	if (AKernelPlayerState* NewPS = Cast<AKernelPlayerState>(PlayerState))
	{
		NewPS->SelectedCharacterIndex = SelectedCharacterIndex;
		NewPS->CharacterDefinition = CharacterDefinition;
	}
}

void AKernelPlayerState::SetBoarded(bool Value)
{
	if (HasAuthority())
	{
		bIsBoarded = Value;
	}
}

void AKernelPlayerState::SetTotalDamageDealt(float NewValue)
{
	if (!HasAuthority()) return;

	TotalDamageDealt = NewValue;

	BroadcastDamageTotal();
}

float AKernelPlayerState::GetTotalDamageDealt()
{
	return TotalDamageDealt;
}

void AKernelPlayerState::OnRep_TotalDamageDealt()
{
	BroadcastDamageTotal();
}

void AKernelPlayerState::BroadcastDamageTotal()
{
	// For UI
	FKernelVerbMessage Message;
	Message.Instigator = this;
	Message.Magnitude  = TotalDamageDealt;

	UGameplayMessageSubsystem::Get(this).BroadcastMessage(TAG_Stats_DamageTotal_Changed, Message);
}

void AKernelPlayerState::BeginPlay()
{
	Super::BeginPlay();
	
	UnlockedCharacterIds.Add("Hero.Blade");
	ServerRPC_ReportUnlockedCharacters(UnlockedCharacterIds);
}

void AKernelPlayerState::ServerRPC_VoteMap_Implementation(int32 MapIndex)
{
	AKernelGameState* GS = GetWorld()->GetGameState<AKernelGameState>();
	if (!GS || GS->GetGamePhase() != EKernelGamePhase::MapSelect) return;

	const auto& Roster = GS->GetMapRoster();
	if (!Roster.IsValidIndex(MapIndex)) return;
	if (Roster[MapIndex] && Roster[MapIndex]->bLocked) return;

	// 투표 상태 갱신 (같은 맵 재클릭이어도 핑은 나가야 하므로 값 비교로 조기 return하지 않음)
	if (VotedMapIndex != MapIndex)
	{
		VotedMapIndex = MapIndex;
		OnRep_VotedMap();
	}

	// 연타 핑 서버 스로틀 1초
	const double Now = GetWorld()->GetTimeSeconds();
	if (Now - LastMapPingTime >= 0.1f)
	{
		LastMapPingTime = Now;
		GS->Multicast_MapPing(GetPlayerName(), MapIndex);
		UE_LOG(LogTemp,Log,TEXT("Ping map"))
	}
}

void AKernelPlayerState::OnRep_VotedMap()
{
	OnMapVoteChanged.Broadcast();
}

void AKernelPlayerState::ServerRPC_ReportUnlockedCharacters_Implementation(const TArray<FName>& UnlockedIds)
{
	AKernelGameState* GS = GetWorld()->GetGameState<AKernelGameState>();
	if (!GS) return;

	UnlockedCharacterIds.Reset();
	for (const FName& Id : UnlockedIds)
	{
		for (const UKernelCharacterDefinition* Def : GS->GetCharacterRoster())
		{
			if (Def && Def->CharacterId == Id)
			{
				UnlockedCharacterIds.AddUnique(Id);
				break;
			}
		}
	}
	OnRep_UnlockedCharacters();
}

void AKernelPlayerState::OnRep_UnlockedCharacters()
{
	OnUnlocksChanged.Broadcast();
}