// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/AffixEntryWidget.h"

#include "Affixes/KernelAffixDefinition.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/RichTextBlock.h"
#include "Components/TextBlock.h"
#include "Item/KernelItemBalanceSettings.h"
#include "Messages/KernelInteractionMessage.h"

void UAffixEntryWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (AffixRollButton)
	{
		AffixRollButton->OnClicked.AddDynamic(this, &ThisClass::HandleRollClicked);
	}
}

void UAffixEntryWidget::SetEntry(const FKernelAffixDetail& Detail, int32 InIndex)
{
	AffixIndex  = InIndex;
	AffixDef    = Detail.AffixDef;      // ⚠ 지금 누락된 부분
	RerollsUsed = Detail.RerollsUsed;

	if (AffixName)        AffixName->SetText(Detail.AffixName);
	if (AffixDescription) AffixDescription->SetText(Detail.AffixDescription);

	if (AffixIcon)
	{
		if (Detail.AffixIcon)
		{
			AffixIcon->SetBrushFromTexture(Detail.AffixIcon);
			AffixIcon->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		else
		{
			AffixIcon->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	UpdateRollButton();
}

void UAffixEntryWidget::SetRerollAllowed(bool bAllowed)
{
	bRerollAllowed = bAllowed;
	UpdateRollButton();
}

void UAffixEntryWidget::ApplyRerollResult(float NewChance, uint8 NewRerollsUsed)
{
	if (AffixDescription && AffixDef)
	{
		AffixDescription->SetText(AffixDef->BuildDescription(NewChance));
	}

	const int32 MaxFree = GetDefault<UKernelItemBalanceSettings>()->FreeRerollsPerAffix;
	if (AffixRollButton)
	{
		AffixRollButton->SetVisibility(NewRerollsUsed < MaxFree
			? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}

void UAffixEntryWidget::UpdateRollButton()
{
	if (!AffixRollButton) return;

	const int32 MaxFree = GetDefault<UKernelItemBalanceSettings>()->FreeRerollsPerAffix;
	const bool bShow = bRerollAllowed && (RerollsUsed < MaxFree);

	AffixRollButton->SetVisibility(
		bShow ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}

void UAffixEntryWidget::HandleRollClicked()
{
	if (AffixIndex == INDEX_NONE || !AffixDef) return;
	OnRerollRequested.ExecuteIfBound(AffixIndex, AffixDef->AffixId);
	UE_LOG(LogTemp,Warning,TEXT("[AffixEntry] Roll selected Affix"));
}