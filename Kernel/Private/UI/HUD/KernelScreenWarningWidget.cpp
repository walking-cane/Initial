// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/KernelScreenWarningWidget.h"

#include "Animation/WidgetAnimation.h"
#include "GameplayAbility/KernelGameplayTags.h"
#include "Components/Image.h"
#include "Messages/KernelVerbMessage.h"

void UKernelScreenWarningWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	SetVisibility(ESlateVisibility::Collapsed);
	
	FWidgetAnimationDynamicEvent EndDelegate;
	EndDelegate.BindDynamic(this, &ThisClass::HandleWarnAnimFinished);
	BindToAnimationFinished(WarnAnim, EndDelegate);

	WarnListenerHandle = UGameplayMessageSubsystem::Get(this).RegisterListener(
		TAG_UI_Warning, this, &ThisClass::HandleWarning);
}

void UKernelScreenWarningWidget::NativeDestruct()
{
	Super::NativeDestruct();
	
	WarnListenerHandle.Unregister();
}

void UKernelScreenWarningWidget::HandleWarning(FGameplayTag Channel, const FKernelVerbMessage& Message)
{
	if (!WarnAnim)
	{
		UE_LOG(LogTemp,Error,TEXT("[WarningWidget] NoAnim"))
		return;
	}

	const float Duration = FMath::Max(Message.Magnitude, 0.1f);
	const float AnimLen  = WarnAnim->GetEndTime();

	SetVisibility(ESlateVisibility::HitTestInvisible);

	// 애니메이션 길이를 실제 경고 시간에 맞춘다
	PlayAnimation(WarnAnim, 0.f, 1, EUMGSequencePlayMode::Forward, AnimLen / Duration);
}

void UKernelScreenWarningWidget::HandleWarnAnimFinished()
{
	UE_LOG(LogTemp,Warning,TEXT("!@@!#!@13233333"))
	SetVisibility(ESlateVisibility::Collapsed);
}