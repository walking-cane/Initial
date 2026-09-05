// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/KernelRunStatsSubsystem.h"
#include "GameplayAbility/KernelGameplayTags.h"
#include "KernelCharacter/KernelPlayerState.h"
#include "Messages/KernelVerbMessage.h"

void UKernelRunStatsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	StatsUpdateListenerHandle = UGameplayMessageSubsystem::Get(this).RegisterListener(
		TAG_Kernel_Damage_Message, this, &ThisClass::OnStatsUpdate);
}

void UKernelRunStatsSubsystem::Deinitialize()
{
	StatsUpdateListenerHandle.Unregister();
	Super::Deinitialize();
}

void UKernelRunStatsSubsystem::OnStatsUpdate(FGameplayTag Channel, const FKernelVerbMessage& Message)
{
	UE_LOG(LogTemp, Warning, TEXT("[RunStats] 수신 — Instigator=%s, Magnitude=%.1f"),
		*GetNameSafe(Message.Instigator), Message.Magnitude);   // ← 맨 위로

	AKernelPlayerState* PS = Cast<AKernelPlayerState>(Message.Instigator);
	if (!PS)
	{
		UE_LOG(LogTemp, Error, TEXT("[RunStats] Instigator가 PlayerState가 아님"));
		return;
	}
	if (!PS->HasAuthority())
	{
		UE_LOG(LogTemp, Error, TEXT("[RunStats] 권위 없음 (클라 인스턴스)"));
		return;
	}

	const FUniqueNetIdRepl Id = PS->GetUniqueId();
	if (!Id.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("[RunStats] UniqueNetId 무효 — PIE에서 흔합니다"));
		return;
	}

	const float NewTotal = TotalDamageByPlayer.FindOrAdd(Id) + Message.Magnitude;
	TotalDamageByPlayer[Id] = NewTotal;
	PS->SetTotalDamageDealt(NewTotal);
}

void UKernelRunStatsSubsystem::ResetRun()
{
	TotalDamageByPlayer.Empty();
}