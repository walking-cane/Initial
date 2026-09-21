// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/KernelSelectArtifactWidget.h"

#include "Artifact/KernelArtifactTypes.h"
#include "Components/HorizontalBox.h"
#include "GameplayAbility/KernelGameplayTags.h"
#include "UI/KernelArtifactEntryWidget.h"

void UKernelSelectArtifactWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	ListenerHandle = UGameplayMessageSubsystem::Get(this).RegisterListener(
		TAG_Artifact_Message_Offered, this, &ThisClass::OfferArtifact);
	
	if (DestructAnim)
	{
		FWidgetAnimationDynamicEvent FinishedEvent;
		FinishedEvent.BindDynamic(this, &UKernelSelectArtifactWidget::OnFadeOutFinished);
		BindToAnimationFinished(DestructAnim, FinishedEvent);
	}
	
	SetVisibility(ESlateVisibility::Collapsed);
}

void UKernelSelectArtifactWidget::NativeDestruct()
{
	ListenerHandle.Unregister();
	
	Super::NativeDestruct();
}

void UKernelSelectArtifactWidget::OfferArtifact(FGameplayTag Channel, const FKernelArtifactOfferMessage& Message)
{
	if (!ArtifactEntryBox || !ArtifactEntryClass) return;
	
	ArtifactEntryBox->ClearChildren();
	UE_LOG(LogTemp,Warning,TEXT("[ArtifactWidget] try to add child"));
	
	for (const FKernelArtifactChoice& Choice : Message.Offer.Choices)
	{
		UKernelArtifactEntryWidget* Entry =
			CreateWidget<UKernelArtifactEntryWidget>(GetOwningPlayer(), ArtifactEntryClass);
		if (!Entry) continue;

		Entry->SetEntry(Choice.Artifact, Message.Offer.OfferId, Choice.ChoiceId);
		Entry->OnClickArtifactEntry.AddUObject(this, &ThisClass::OnArtifactClicked);
		
		AddEntryArray(Entry);
		
		ArtifactEntryBox->AddChild(Entry);
		UE_LOG(LogTemp,Warning,TEXT("[ArtifactWidget] AddChild"));
	}
	
	SetVisibility(ESlateVisibility::Visible);
	
	if (APlayerController* PC = GetOwningPlayer())
	{
		PC->SetInputMode(FInputModeUIOnly());
		PC->SetShowMouseCursor(true);
	}
	
	PlayAnimation(ConstructAnim);
}

void UKernelSelectArtifactWidget::OnArtifactClicked(UKernelArtifactEntryWidget* Clicked)
{
	for (UKernelArtifactEntryWidget* Entry : EntryArray)
	{
		if (!Entry) continue;

		if (Entry == Clicked) { Entry->OnPicked(); }
		else                  { Entry->OnUnPicked(); }

		Entry->SetSelectEnabled(false);   // 중복 클릭 방지 — 아래 설명
	}
	
	if (APlayerController* PC = GetOwningPlayer())
	{
		PC->SetInputMode(FInputModeGameOnly());
		PC->SetShowMouseCursor(false);
	}
	
	PlayAnimation(DestructAnim);
}

void UKernelSelectArtifactWidget::AddEntryArray(UKernelArtifactEntryWidget* Entry)
{
	EntryArray.Add(Entry);
}

void UKernelSelectArtifactWidget::OnFadeOutFinished()
{
	ArtifactEntryBox->ClearChildren();
	EntryArray.Reset();
	SetVisibility(ESlateVisibility::Collapsed);
}
