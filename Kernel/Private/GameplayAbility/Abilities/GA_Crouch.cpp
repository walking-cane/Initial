// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbility/Abilities/GA_Crouch.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayAbility/KernelGameplayTags.h"

void UGA_Crouch::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                 const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (!ActorInfo)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	if (GetAbilitySystemComponentFromActorInfo()->HasMatchingGameplayTag(TAG_Status_Sprint))
	{
		if (MultiplySpeedEffectClass)
		{
			FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(MultiplySpeedEffectClass);
			if (SpecHandle.IsValid())
			{
				SpeedHandle = 
					ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
			}
		}
		
		if (SlideMontage)
		{
			UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(	
				this, NAME_None, SlideMontage.Get());
			MontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnMontageCancelled);
			MontageTask->ReadyForActivation();
		}
		
		UE_LOG(LogTemp,Log,TEXT("[GA_Crouch] Set to slide"))
		return;
	}
	
	if (ACharacter* Char = Cast<ACharacter>(GetAvatarActorFromActorInfo()))
	{
		Char->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		UE_LOG(LogTemp,Log,TEXT("[GA_Crouch] Set to crouch"))
	}
}

void UGA_Crouch::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
	
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_Crouch::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Crouch::OnMontageCancelled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}
