// Fill out your copyright notice in the Description page of Project Settings.


#include "Tutorial/KernelSCP_035.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "LevelSequenceActor.h"
#include "LevelSequencePlayer.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameplayAbility/KernelAbilitySystemComponent.h"
#include "GameplayAbility/KernelGameplayTags.h"
#include "Messages/KernelVerbMessage.h"

// Sets default values
AKernelSCP_035::AKernelSCP_035()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	DefaultRoot = CreateDefaultSubobject<USceneComponent>("DefaultRoot");
	SetRootComponent(DefaultRoot);
	
	SadMesh = CreateDefaultSubobject<UStaticMeshComponent>("SadMesh");
	HappyMesh1 = CreateDefaultSubobject<UStaticMeshComponent>("HappyMesh");
	
	SadMesh->SetupAttachment(DefaultRoot);
	HappyMesh1->SetupAttachment(DefaultRoot);
	
	SadMesh->SetVisibility(true);
	HappyMesh1->SetVisibility(false);
	
	SadMesh->SetCustomDepthStencilValue(5);
}

// Called when the game starts or when spawned
void AKernelSCP_035::BeginPlay()
{
	Super::BeginPlay();
}

void AKernelSCP_035::OnFocus(APawn* FocusingPawn)
{
	if (!FocusingPawn || !FocusingPawn->IsLocallyControlled()) return;
	
	SadMesh->SetRenderCustomDepth(true);
	
	UGameplayMessageSubsystem::Get(this)
		.BroadcastMessage(TAG_Interact_OnFocus, FKernelInteractionMessage());
}

void AKernelSCP_035::OnEndFocus(APawn* FocusingPawn)
{
	if (!FocusingPawn || !FocusingPawn->IsLocallyControlled()) return;
	
	SadMesh->SetRenderCustomDepth(false);
	
	UGameplayMessageSubsystem::Get(this)
		.BroadcastMessage(TAG_Interact_EndFocus, FKernelInteractionMessage());
}

void AKernelSCP_035::OnInteract(APawn* InteractingPawn)
{
	if (!InteractingPawn || bPlayed) return;
	
	CachedPC = Cast<APlayerController>(InteractingPawn->GetController());
	if (!CachedPC) return;
	
	if (!LevelSequence || !LevelSequence->GetSequencePlayer())
	{
		UE_LOG(LogTemp, Error, TEXT("[ArtifactProp] SequenceActor 미지정"));
		return;
	}
	
	bPlayed = true;
	SadMesh->SetRenderCustomDepth(false);
	
	//CachedPC->SetIgnoreMoveInput(true);
	//CachedPC->SetIgnoreLookInput(true);
	
	ULevelSequencePlayer* Player = LevelSequence->GetSequencePlayer();
	Player->Play();
	
	//Sequence error safety
	FTimerHandle SafetyHandle;
	GetWorldTimerManager().SetTimer(SafetyHandle, this, &ThisClass::OnSequenceFinished,
		Player->GetDuration().AsSeconds() + 1.f, false);
	
	if (UKernelAbilitySystemComponent* ASC = 
		Cast<UKernelAbilitySystemComponent>(
			UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InteractingPawn)))
	{
		if (GrantAbilitySet)
		{
			GrantAbilitySet->GiveToAbilitySystem(ASC, &GrantedHandles, this);
		}
	}
}

void AKernelSCP_035::OnSequenceFinished()
{
	CachedPC->SetIgnoreMoveInput(false);
	CachedPC->SetIgnoreLookInput(false);
}

void AKernelSCP_035::StartCameraFade(bool bReverse, float FadeDuration)
{
	if (CachedPC && CachedPC->PlayerCameraManager)
	{
		CachedPC->PlayerCameraManager->StartCameraFade(
			bReverse ? 1.f : 0.f,
			bReverse ? 0.f : 1.f,
			FadeDuration, 
			FLinearColor::Black,
			true,
			/*bHoldWhenFinished=*/ true);
	}
}

void AKernelSCP_035::SendFogEvent()
{
	if (CachedPC)
	{
		UE_LOG(LogTemp,Log,TEXT("[SCP035] Send Fog"))
		FKernelVerbMessage Message;
		Message.Magnitude = -1.f;
		
		UGameplayMessageSubsystem::Get(this).BroadcastMessage(
		TAG_UI_Border, Message);
	}
}

void AKernelSCP_035::SendAngerEvent()
{
	if (CachedPC)
	{
		UE_LOG(LogTemp,Log,TEXT("[SCP035] Send Anger"))
		FKernelVerbMessage Message;
		Message.Magnitude = 1.f;
		
		UGameplayMessageSubsystem::Get(this).BroadcastMessage(
		TAG_UI_Border, Message);
	}
}

void AKernelSCP_035::SendFadeOutEvent()
{
	if (CachedPC)
	{
		UE_LOG(LogTemp,Log,TEXT("[SCP035] Send Fade out"))
		FKernelVerbMessage Message;
		Message.Magnitude = 0.f;
		
		UGameplayMessageSubsystem::Get(this).BroadcastMessage(
		TAG_UI_Border, Message);
		
		//CachedPC->SetIgnoreMoveInput(false);
		//CachedPC->SetIgnoreLookInput(false);
	}
}

void AKernelSCP_035::StartCameraShake()
{
	if (CachedPC)
	{
		CachedPC->ClientStartCameraShake(CameraShake);
		
		// Send Change Stencil value to 4 to EnemyCharacter 
		FKernelVerbMessage Message;
		Message.Magnitude = 4.f;
		UGameplayMessageSubsystem::Get(this).BroadcastMessage(
			TAG_Gameplay_ChangeOutline, Message);
	}
}

void AKernelSCP_035::StopCameraShake()
{
	if (CachedPC)
	{
		CachedPC->ClientStopCameraShake(CameraShake);
	}
}

void AKernelSCP_035::StartLetterBox()
{
	if (CachedPC)
	{
		FKernelVerbMessage Message;
		Message.Magnitude = 2.f;
		
		UGameplayMessageSubsystem::Get(this).BroadcastMessage(
		TAG_UI_Border, Message);
	}
}
