// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbility/Abilities/GA_Interact.h"
#include "AbilitySystemLog.h"
#include "Interaction/KernelInteractionComponent.h"
#include "Interaction/KernelInteractionInterface.h"

UGA_Interact::UGA_Interact()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void UGA_Interact::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                   const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	UE_LOG(LogAbilitySystem, Warning, TEXT("[GA_Interact] ActivateAbility"))
	
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, false);
		return;
	}
	
	if (APawn* Pawn = Cast<APawn>(GetAvatarActorFromActorInfo()))
	{
		auto* InteractionComponent = Pawn->FindComponentByClass<UKernelInteractionComponent>();
		
		if (InteractionComponent && InteractionComponent->CurrentTarget)
		{
			InteractionComponent->CurrentTarget->OnInteract(Pawn);
			UE_LOG(LogTemp,Log,TEXT("[GA_Interact] InteractionTarget : %s"),
				*InteractionComponent->CurrentTarget->GetInteractName().ToString());
			EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
			return;
		}
	}
	
	CancelAbility(Handle, ActorInfo, ActivationInfo, false);
}
