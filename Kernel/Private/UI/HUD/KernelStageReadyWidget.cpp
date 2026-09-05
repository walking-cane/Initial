// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/HUD/KernelStageReadyWidget.h"
#include "GameplayAbility/KernelGameplayTags.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "Messages/KernelInteractionMessage.h"
#include "UI/HUD/KernelReadyBox.h"

void UKernelStageReadyWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	StageReadyListenerHandle = UGameplayMessageSubsystem::Get(this).RegisterListener(
		TAG_Interact_PlayerReady, this, &ThisClass::RefreshReadyState);
	DeActivateListenerHandle = UGameplayMessageSubsystem::Get(this).RegisterListener(
		FGameplayTag::RequestGameplayTag("Interact.EndFocus"), this, &ThisClass::DeActivateWidget);

	SetVisibility(ESlateVisibility::Collapsed);
}

void UKernelStageReadyWidget::NativeDestruct()
{
	Super::NativeDestruct();
	
	StageReadyListenerHandle.Unregister();
	DeActivateListenerHandle.Unregister();
}

void UKernelStageReadyWidget::EnsureBoxCount(int32 Count)
{
	if (!ReadyBoxContainer || !ReadyBoxClass) return;
	if (ReadyBoxes.Num() == Count) return;   // 변화 없으면 재생성하지 않는다

	ReadyBoxContainer->ClearChildren();
	ReadyBoxes.Empty();

	for (int32 i = 0; i < Count; ++i)
	{
		if (UKernelReadyBox* Box = CreateWidget<UKernelReadyBox>(GetOwningPlayer(), ReadyBoxClass))
		{
			ReadyBoxContainer->AddChild(Box);
			ReadyBoxes.Add(Box);
			
			UE_LOG(LogTemp, Log, TEXT("[ReadyWidget] Added box"))
		}
	}
}

void UKernelStageReadyWidget::RefreshReadyState(FGameplayTag Channel, const FKernelReadyCountMessage& Message)
{
	if (Message.ReadyCount <= 0 || Message.TotalCount <= 0)
	{
		SetVisibility(ESlateVisibility::Collapsed);
		return;
	}
	
	EnsureBoxCount(Message.TotalCount);
	
	for (int32 i = 0; i < ReadyBoxes.Num(); ++i)
	{
		if (ReadyBoxes[i]) ReadyBoxes[i]->SetChecked(i < Message.ReadyCount);
	}
	
	if (Message.bIsLocalPlayerReady)
	{
		ReadyBoxContainer->SetRenderOpacity(1.f);
	}
	else
	{
		ReadyBoxContainer->SetRenderOpacity(0.3f);
	}
	
	SetVisibility(ESlateVisibility::HitTestInvisible);
	UE_LOG(LogTemp, Log, TEXT("[ReadyWidget] Appears!"))
}

void UKernelStageReadyWidget::DeActivateWidget(FGameplayTag Channel, const FKernelInteractionMessage& Message)
{
	for (int32 i = 0; i < ReadyBoxes.Num(); ++i)
	{
		if (ReadyBoxes[i]) ReadyBoxes[i]->SetChecked(false); // 체크 해제 후 위젯 제거.
	}
	
	SetVisibility(ESlateVisibility::Collapsed);
}
