// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/NPCs/KernelNPC_RollAffix.h"

#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameplayAbility/KernelGameplayTags.h"
#include "Messages/KernelVerbMessage.h"

// Sets default values
AKernelNPC_RollAffix::AKernelNPC_RollAffix()
{
	NPCMesh = CreateDefaultSubobject<USkeletalMeshComponent>("NPCMesh");
	NPCMesh->SetRenderCustomDepth(false);
	NPCMesh->SetCustomDepthStencilValue(5);
	SetRootComponent(NPCMesh);
	
	bReplicates = true;
	
	PrimaryActorTick.bCanEverTick = false;
}

void AKernelNPC_RollAffix::OnFocus(APawn* FocusingPawn)
{
	if (!FocusingPawn->IsLocallyControlled()) return;
	
	if (NPCMesh)
	{
		NPCMesh->SetRenderCustomDepth(true);
	}
	
	UGameplayMessageSubsystem::Get(this).BroadcastMessage(
		TAG_Interact_OnFocus, FKernelInteractionMessage());
}

void AKernelNPC_RollAffix::OnEndFocus(APawn* FocusingPawn)
{
	if (!FocusingPawn->IsLocallyControlled()) return;
	
	if (NPCMesh)
	{
		NPCMesh->SetRenderCustomDepth(false);
	}
	
	UGameplayMessageSubsystem::Get(this).BroadcastMessage(
		TAG_Interact_EndFocus, FKernelInteractionMessage());
}

void AKernelNPC_RollAffix::OnInteract(APawn* InteractingPawn)
{
	if (!InteractingPawn->IsLocallyControlled()) return;
	
	UGameplayMessageSubsystem::Get(this).BroadcastMessage(
		TAG_UI_RollAffix, FKernelInteractionMessage());
	
	FKernelVerbMessage VerbMessage;
	VerbMessage.Magnitude = -1.f;
	
	UGameplayMessageSubsystem::Get(this).BroadcastMessage(
		TAG_UI_ToggleLayout, VerbMessage);
		
	UE_LOG(LogTemp,Warning,TEXT("[RollNPC] RollAffix sent"))
}

