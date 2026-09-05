// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/KernelMinimalTooltipWidget.h"
#include "Messages/KernelInteractionMessage.h"
#include "Components/TextBlock.h"

void UKernelMinimalTooltipWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	FocusListenerHandle = UGameplayMessageSubsystem::Get(this).RegisterListener(
	FGameplayTag::RequestGameplayTag("Interact.Focus"), this, &ThisClass::OnFocus);
	EndFocusListenerHandle = UGameplayMessageSubsystem::Get(this).RegisterListener(
		FGameplayTag::RequestGameplayTag("Interact.EndFocus"), this, &ThisClass::OnEndFocus);
	
	SetVisibility(ESlateVisibility::Collapsed);
}

void UKernelMinimalTooltipWidget::NativeDestruct()
{
	Super::NativeDestruct();
	
	FocusListenerHandle.Unregister();
	EndFocusListenerHandle.Unregister();
}

void UKernelMinimalTooltipWidget::OnFocus(FGameplayTag Channel, const FKernelInteractionMessage& Message)
{
	if (InteractText)
	{
		InteractText->SetText(Message.Description);
	}
	
	SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UKernelMinimalTooltipWidget::OnEndFocus(FGameplayTag Channel, const FKernelInteractionMessage& Message)
{
	SetVisibility(ESlateVisibility::Collapsed);
}