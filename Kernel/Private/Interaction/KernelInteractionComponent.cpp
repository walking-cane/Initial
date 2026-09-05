// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/KernelInteractionComponent.h"
#include "Game/KernelGameState.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameplayAbility/KernelGameplayTags.h"
#include "Interaction/KernelInteractionInterface.h"
#include "Messages/KernelInteractionMessage.h"

// Sets default values for this component's properties
UKernelInteractionComponent::UKernelInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UKernelInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
	
	SetInteractionEnabled(true);
	TryBindToGameState();
}

void UKernelInteractionComponent::TryBindToGameState()
{
	AKernelGameState* GS = GetWorld()->GetGameState<AKernelGameState>();
	if (!GS)
	{
		// 클라 초기엔 GameState 복제 전일 수 있음, 풀링체크
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::TryBindToGameState);
		return;
	}

	GS->OnGamePhaseChanged.AddUObject(this, &UKernelInteractionComponent::HandleGamePhase);
	HandleGamePhase(GS->GetGamePhase());
}

void UKernelInteractionComponent::HandleGamePhase(EKernelGamePhase Phase)
{
	if (Phase == EKernelGamePhase::Departing)
	{
		SetInteractionEnabled(false);
	}
}

void UKernelInteractionComponent::SetInteractionEnabled(bool bEnabled)
{
	UWorld* World = GetWorld();
	if (!World) return;

	if (bEnabled)
	{
		World->GetTimerManager().SetTimer(InteractionTimerHandle, this,
			&ThisClass::TraceForInteraction, 0.1f, true);
		return;
	}

	World->GetTimerManager().ClearTimer(InteractionTimerHandle);

	if (CurrentTarget)
	{
		APawn* Pawn = Cast<APawn>(GetOwner());
		CurrentTarget->OnEndFocus(Pawn);
		CurrentTarget = nullptr;
		CurrentTargetActor = nullptr;

		if (Pawn && Pawn->IsLocallyControlled())
		{
			BroadcastEndFocus();
		}
	}
}

void UKernelInteractionComponent::TraceForInteraction()
{
	if (CurrentTarget && !CurrentTargetActor.IsValid())
	{
		CurrentTarget = nullptr;
		CurrentTargetActor = nullptr;
	}
	
	APawn* Pawn = Cast<APawn>(GetOwner());
	if (!Pawn) return;

	if (!Pawn->IsLocallyControlled() && !Pawn->HasAuthority()) return;

	const FVector Start = Pawn->GetPawnViewLocation();
	const FRotator ViewRot = Pawn->GetViewRotation();
	const FVector End = Start + (ViewRot.Vector() * 200.0f);
	
	FHitResult HitResult;
	FCollisionQueryParams Params;
	FCollisionShape Sphere = FCollisionShape::MakeSphere(30.0f);
	Params.AddIgnoredActor(Pawn);
	
	bool bHit = GetWorld()->SweepSingleByChannel(
		HitResult, Start, End, FQuat::Identity, ECC_GameTraceChannel2, Sphere, Params
	);
	
	//DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, .5f);
	
	if (bHit && HitResult.GetActor())
	{
		if (IKernelInteractionInterface* Target = Cast<IKernelInteractionInterface>(HitResult.GetActor()))
		{
			if (CurrentTarget == Target) return;
			
			CurrentTargetActor = HitResult.GetActor();
			CurrentTarget = Target;
			CurrentTarget->OnFocus(Pawn);
			
			if (Pawn->IsLocallyControlled())
			{
				BroadcastFocus(CurrentTarget);
			}
			
			return;
		}
	}

	if (CurrentTarget != nullptr)
	{
		CurrentTarget->OnEndFocus(Pawn);
		CurrentTarget = nullptr;
		CurrentTargetActor = nullptr;
		
		if (Pawn->IsLocallyControlled())
		{
			BroadcastEndFocus();
		}
	}
}

void UKernelInteractionComponent::BroadcastFocus(IKernelInteractionInterface* Target)
{
	if (Target)
	{
		FKernelInteractionMessage Message;
		Message.DisplayName = Target->GetInteractName();
		Message.Description = Target->GetInteractDescription();
		Message.DisplayIcon = Target->GetInteractIcon();
		Message.TargetLocation = Target->GetInteractLocation();
		Message.Details = Target->GetInteractDetails();
		Message.WidgetClass = Target->GetInteractWidgetClass();
		
		if (Target->GetInteractIcon())
		{
			UGameplayMessageSubsystem::Get(this).
				BroadcastMessage(FGameplayTag::RequestGameplayTag("Interact.ItemFocus"), Message);
		}
		else
		{
			UGameplayMessageSubsystem::Get(this).BroadcastMessage(
				TAG_Interact_OnFocus, Message);
		}
	}
}

void UKernelInteractionComponent::BroadcastEndFocus()
{
	FKernelInteractionMessage Message;
	
	UGameplayMessageSubsystem::Get(this).
	BroadcastMessage(TAG_Interact_EndFocus, Message);
}

void UKernelInteractionComponent::EndPlay(const EEndPlayReason::Type Reason)
{
	if (AKernelGameState* GS = GetWorld()->GetGameState<AKernelGameState>())
	{
		GS->OnGamePhaseChanged.RemoveAll(this);
	}
	Super::EndPlay(Reason);
}