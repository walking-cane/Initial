// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/KernelArtifactEntryWidget.h"
#include "Artifact/KernelArtifactDefinition.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "KernelCharacter/KernelPlayerController.h"

void UKernelArtifactEntryWidget::SetEntry(
	const UKernelArtifactDefinition* Def, int32 InOfferId, int32 InChoiceId)
{
	if (!Def)
	{
		UE_LOG(LogTemp, Error, TEXT("[ArtifactEntry] Definition is null"));
		return;
	}
	
	OfferId  = InOfferId;
	ChoiceId = InChoiceId;
	
	ArtifactImage->SetBrushFromTexture(Def->Icon.LoadSynchronous());
	Name->SetText(Def->DisplayName);
	Description->SetText(Def->Description);
	
	FeatureTextBox->ClearChildren();
	for (const EKernelArtifactType Type : Def->Types)
	{
		UTextBlock* Text = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
		Text->SetText(GetArtifactTypeText(Type));
		FeatureTextBox->AddChild(Text);
	}
}

void UKernelArtifactEntryWidget::OnPicked()
{
	PlayAnimation(PickedAnim);
}

void UKernelArtifactEntryWidget::OnUnPicked()
{
	PlayAnimation(UnPickedAnim);
}

void UKernelArtifactEntryWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (SelectButton)
	{
		SelectButton->OnClicked.AddDynamic(this, &ThisClass::OnSelectClicked);
	}
}

void UKernelArtifactEntryWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UKernelArtifactEntryWidget::OnSelectClicked()
{
	if (OfferId == INDEX_NONE || ChoiceId == INDEX_NONE) return;

	if (AKernelPlayerController* PC = Cast<AKernelPlayerController>(GetOwningPlayer()))
	{
		PC->Server_ConfirmArtifactChoice(OfferId, ChoiceId);
		PC->SetInputMode(FInputModeUIOnly());
		PC->SetShowMouseCursor(true);
	}
	
	OnClickArtifactEntry.Broadcast(this);
	UE_LOG(LogTemp,Warning,TEXT("[ArtifactEntry] Picked Index : %i"), EntryIndex);
}