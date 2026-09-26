// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/KernelBorderWidget.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/VerticalBox.h"
#include "GameplayAbility/KernelGameplayTags.h"
#include "Messages/KernelVerbMessage.h"

void UKernelBorderWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	SetVisibility(ESlateVisibility::Collapsed);
	
	BorderListenerHandle = UGameplayMessageSubsystem::Get(this).RegisterListener(
		TAG_UI_Border, this, &ThisClass::ToggleBorder);
	
}

void UKernelBorderWidget::NativeDestruct()
{
	BorderListenerHandle.Unregister();
	
	Super::NativeDestruct();
}

void UKernelBorderWidget::ToggleBorder(FGameplayTag Channel, const FKernelVerbMessage& Message)
{
	float Mag = Message.Magnitude;
	
	if (Mag > 0.9f && Mag < 1.1f)
	{
		UE_LOG(LogTemp,Log,TEXT("[BorderWidget] Set Anger"))
		
		FogOverlay->SetVisibility(ESlateVisibility::Collapsed);
		AngerOverlay->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		PlayAnimationForward(AngerAnim);
		
		return;
	}
	
	if (Mag < 0)
	{
		UE_LOG(LogTemp,Log,TEXT("[BorderWidget] Set Fog"))
		
		FogOverlay->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		AngerOverlay->SetVisibility(ESlateVisibility::Collapsed);
		PlayAnimationForward(FogAnim);
		
		return;
	}
	
	if (Mag == 0)
	{
		UE_LOG(LogTemp,Log,TEXT("[BorderWidget] Collapsed"))
		
		PlayAnimationReverse(FogAnim);
		PlayAnimationReverse(LetterBoxAnim);
	}
	
	if (Mag > 1.5f && Mag < 2.5f)
	{
		UE_LOG(LogTemp,Log,TEXT("[BorderWidget] Letterbox"))
		
		LetterBox->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		SetVisibility(ESlateVisibility::HitTestInvisible);
		PlayAnimationForward(LetterBoxAnim);
	}
}
