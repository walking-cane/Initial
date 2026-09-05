// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/GameplayLayout.h"
#include "GameplayAbility/KernelGameplayTags.h"
#include "Components/PanelWidget.h"
#include "Messages/KernelVerbMessage.h"

void UGameplayLayout::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (!GetOwningPlayer()) return;
	
	ToggleListenerHandle = UGameplayMessageSubsystem::Get(this).RegisterListener(TAG_UI_ToggleLayout, this, &ThisClass::ToggleGameplayLayout);
}

void UGameplayLayout::NativeDestruct()
{
	Super::NativeDestruct();
	
	ToggleListenerHandle.Unregister();
}

void UGameplayLayout::ToggleGameplayLayout(FGameplayTag Channel, const FKernelVerbMessage& Message)
{
	Message.Magnitude > 0 ? SetVisibility(ESlateVisibility::SelfHitTestInvisible) : SetVisibility(ESlateVisibility::Collapsed);
}
