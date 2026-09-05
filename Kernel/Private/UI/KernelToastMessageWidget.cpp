// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/KernelToastMessageWidget.h"

#include "Components/TextBlock.h"
#include "GameplayAbility/KernelGameplayTags.h"
#include "Messages/KernelInteractionMessage.h"

void UKernelToastMessageWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	FWidgetAnimationDynamicEvent Finished;
	Finished.BindDynamic(this, &UKernelToastMessageWidget::HandleAnimFinished);
	BindToAnimationFinished(ToastAnim, Finished);
	
	ToastListenerHandle = UGameplayMessageSubsystem::Get(this).RegisterListener(
		TAG_UI_ToastMessage, this, &ThisClass::OnActive);
	
	SetVisibility(ESlateVisibility::Collapsed); 
}

void UKernelToastMessageWidget::NativeDestruct()
{
	ToastListenerHandle.Unregister();
	Super::NativeDestruct();
}

void UKernelToastMessageWidget::OnActive(FGameplayTag Channel, const FKernelToastMessage& Message)
{
	ToastMessage->SetText(Message.ToastMessage);
	ToastMessage->SetColorAndOpacity(Message.IsWarning
		? FSlateColor(FLinearColor(1.f, 0.3f, 0.2f))
		: FSlateColor(FLinearColor::White));
	
	SetVisibility(ESlateVisibility::HitTestInvisible);
	
	if (ToastAnim)
	{
		PlayAnimation(ToastAnim);
	}
	else
	{
		HandleAnimFinished();   // 애니메이션 없으면 즉시 정리 (폴백)
	}
}

void UKernelToastMessageWidget::HandleAnimFinished()
{
	SetVisibility(ESlateVisibility::Collapsed);
}
