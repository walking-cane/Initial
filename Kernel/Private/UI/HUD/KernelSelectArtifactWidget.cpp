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
	
	for (const FKernelArtifactChoice& Choice : Message.Offer.Choices)
	{
		UKernelArtifactEntryWidget* Entry =
			CreateWidget<UKernelArtifactEntryWidget>(GetOwningPlayer(), ArtifactEntryClass);
		if (!Entry) continue;

		Entry->SetEntry(Choice.Artifact, Message.Offer.OfferId, Choice.ChoiceId);
		ArtifactEntryBox->AddChild(Entry);
	}
	
	SetVisibility(ESlateVisibility::Visible);
	
	if (APlayerController* PC = GetOwningPlayer())
	{
		PC->SetInputMode(FInputModeUIOnly());
		PC->SetShowMouseCursor(true);
	}
}
