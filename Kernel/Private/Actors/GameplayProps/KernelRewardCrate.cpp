// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/GameplayProps/KernelRewardCrate.h"

#include "Components/TimelineComponent.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameplayAbility/KernelGameplayTags.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AKernelRewardCrate::AKernelRewardCrate()
{
	bReplicates = true;
	PrimaryActorTick.bCanEverTick = true;
	
	CrateMesh = CreateDefaultSubobject<UStaticMeshComponent>("CrateMesh");
	CrateMesh->SetCustomDepthStencilValue(5);
	SetRootComponent(CrateMesh);
}

void AKernelRewardCrate::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AKernelRewardCrate, bIsActive);
}

void AKernelRewardCrate::InitializeObject()
{
	CrateMesh->SetCollisionProfileName("NoCollision");
	SetActorEnableCollision(false);
	
	OriginLoc = GetActorLocation();
}

void AKernelRewardCrate::BeginPlay()
{
	Super::BeginPlay();
	
	InitializeObject();
}

// Called every frame
void AKernelRewardCrate::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (!bIsActive) return;
	
	FloatTime += DeltaTime;

	const float Offset = FMath::Sin(FloatTime * FloatSpeed) * FloatHeight;
	SetActorLocation(OriginLoc + FVector(0.f, 0.f, Offset));
	AddActorLocalRotation(FRotator(0.f, RotAmount * DeltaTime, 0.f));
}

void AKernelRewardCrate::OnWaveCleared()
{
	bIsActive = true;
	OnRep_IsActive();
}

void AKernelRewardCrate::OnFocus(APawn* FocusingPawn)
{
	if (!FocusingPawn || !FocusingPawn->IsLocallyControlled()) return;
	
	CrateMesh->SetRenderCustomDepth(true);
	
	UGameplayMessageSubsystem::Get(this).BroadcastMessage(
		TAG_Interact_OnFocus, FKernelInteractionMessage());
}

void AKernelRewardCrate::OnEndFocus(APawn* FocusingPawn)
{
	if (!FocusingPawn || !FocusingPawn->IsLocallyControlled()) return;
	
	CrateMesh->SetRenderCustomDepth(false);
	
	UGameplayMessageSubsystem::Get(this).BroadcastMessage(
	TAG_Interact_EndFocus, FKernelInteractionMessage());
}

void AKernelRewardCrate::OnInteract(APawn* InteractingPawn)
{
	if (!InteractingPawn) return;
}

FText AKernelRewardCrate::GetInteractName()
{
	return FText::FromString("RewardCrate");
}

void AKernelRewardCrate::OnRep_IsActive()
{
	StartDissolve();
	SetActorEnableCollision(true);
	CrateMesh->SetCollisionProfileName("Interactable");
	
	FKernelToastMessage Message;
	Message.IsWarning = false;
	Message.ToastMessage = FText::FromString(TEXT("보상을 획득하세요!"));
	
	UGameplayMessageSubsystem::Get(this).BroadcastMessage(
		TAG_UI_ToastMessage, Message);
}