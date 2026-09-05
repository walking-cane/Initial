// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/HUD/ItemDescriptionWidget.h"
#include "Messages/KernelInteractionMessage.h"
#include "Components/Image.h"
#include "Components/PanelWidget.h"
#include "Components/TextBlock.h"
#include "GameplayAbility/KernelGameplayTags.h"
#include "Item/KernelItemBalanceSettings.h"
#include "UI/AffixEntryWidget.h"

void UItemDescriptionWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	SetVisibility(ESlateVisibility::Collapsed);
	
	FocusListenerHandle = UGameplayMessageSubsystem::Get(this).RegisterListener(
		FGameplayTag::RequestGameplayTag("Interact.ItemFocus"), this, &ThisClass::OnFocus);
	EndFocusListenerHandle = UGameplayMessageSubsystem::Get(this).RegisterListener(
		TAG_Interact_EndFocus, this, &ThisClass::OnEndFocus);
}

void UItemDescriptionWidget::NativeDestruct()
{
	FocusListenerHandle.Unregister();
	EndFocusListenerHandle.Unregister();
	
	Super::NativeDestruct();
}

void UItemDescriptionWidget::OnFocus(FGameplayTag Channel, const FKernelInteractionMessage& Message)
{
	// 내용을 먼저 채우고 나서 표시 (한 프레임 이전 정보가 비치는 걸 방지)
	if (ItemName) ItemName->SetText(Message.DisplayName);
	if (ItemDescription) ItemDescription->SetText(Message.Description);
	if (ItemImage && Message.DisplayIcon) ItemImage->SetBrushFromTexture(Message.DisplayIcon);
	
	if (AffixEntryBox)
	{
		AffixEntryBox->ClearChildren();

		if (AffixEntryClass)
		{
			for (int32 i = 0; i < Message.Details.Num(); ++i)
			{
				if (UAffixEntryWidget* Entry = CreateWidget<UAffixEntryWidget>(GetOwningPlayer(), AffixEntryClass))
				{
					Entry->SetEntry(Message.Details[i], i);
					// SetRerollAllowed를 부르지 않음 → 버튼 자동 숨김
					AffixEntryBox->AddChild(Entry);
				}
			}
		}
	}
	
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	AffixEntryBox->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UItemDescriptionWidget::OnEndFocus(FGameplayTag Channel, const FKernelInteractionMessage& Message)
{
	SetVisibility(ESlateVisibility::Collapsed);
	AffixEntryBox->SetVisibility(ESlateVisibility::Collapsed);
}

void UItemDescriptionWidget::HandleAffixRerolled(int32 AffixIndex, const FKernelAppliedAffix& NewApplied)
{
	if (!AffixEntryBox) return;

	for (int32 i = 0; i < AffixEntryBox->GetChildrenCount(); ++i)
	{
		UAffixEntryWidget* Row = Cast<UAffixEntryWidget>(AffixEntryBox->GetChildAt(i));
		if (Row && Row->GetAffixIndex() == AffixIndex)
		{
			Row->ApplyRerollResult(NewApplied.RolledChance, NewApplied.RerollsUsed);
			break;
		}
	}
}