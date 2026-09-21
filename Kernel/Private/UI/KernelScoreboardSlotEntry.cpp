// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/KernelScoreboardSlotEntry.h"

#include "Artifact/KernelArtifactComponent.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "GameplayAbility/KernelGameplayTags.h"
#include "KernelCharacter/KernelPlayerState.h"
#include "KernelCharacter/Hero/KernelCharacterDefinition.h"
#include "Messages/KernelVerbMessage.h"

void UKernelScoreboardSlotEntry::NativeConstruct()
{
	Super::NativeConstruct();
	
	DamageListenerHandle = UGameplayMessageSubsystem::Get(this).RegisterListener(
		TAG_Stats_DamageTotal_Changed, this, &ThisClass::OnDealtChanged);
}

void UKernelScoreboardSlotEntry::NativeDestruct()
{
	DamageListenerHandle.Unregister();
	
	Super::NativeDestruct();
}

void UKernelScoreboardSlotEntry::OnDealtChanged(FGameplayTag Channel, const FKernelVerbMessage& Message)
{
	if (Message.Magnitude > 0)
	{
		DamageText->SetText(FText::AsNumber(Message.Magnitude));
	}
}

void UKernelScoreboardSlotEntry::SetEntry(const AKernelPlayerState* PS)
{
	if (!PS || !PS->GetCharacterDefinition()) return;
	
	PlayerPortrait->SetBrushFromTexture(PS->GetCharacterDefinition()->Portrait.LoadSynchronous());
	PlayerName->SetText(FText::FromString(PS->GetPlayerName()));
	
	if (PS->FindComponentByClass<UKernelArtifactComponent>())
	{
		UpdateArtifactEntry(PS);
	}
	
	// PlayerWeapon->SetBrushFromTexture(PS->)
}

void UKernelScoreboardSlotEntry::UpdateArtifactEntry(const AKernelPlayerState* PS)
{
	
}
