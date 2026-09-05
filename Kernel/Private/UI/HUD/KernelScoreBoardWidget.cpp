// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/KernelScoreBoardWidget.h"

#include "Components/TextBlock.h"
#include "GameplayAbility/KernelGameplayTags.h"
#include "Messages/KernelVerbMessage.h"

void UKernelScoreBoardWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	StatsUpdateListenerHandle = UGameplayMessageSubsystem::Get(this).RegisterListener(
		TAG_Stats_DamageTotal_Changed, this, &ThisClass::OnStatsChanged);
}

void UKernelScoreBoardWidget::NativeDestruct()
{
	StatsUpdateListenerHandle.Unregister();
	Super::NativeDestruct();
}

void UKernelScoreBoardWidget::OnStatsChanged(FGameplayTag Channel, const FKernelVerbMessage& Message)
{
	if (TotalDamageDealt)
	{
		TotalDamageDealt->SetText(FText::AsNumber(Message.Magnitude));
	}
}
