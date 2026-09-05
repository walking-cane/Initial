 // Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbility/Abilities/GA_Sprint.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemLog.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"

 void UGA_Sprint::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                  const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
 	UE_LOG(LogAbilitySystem, Warning, TEXT("GA_Sprint :: Activate"))
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
 	
 	if (SprintEffectClass)
 	{
 		ActiveSprintEffectHandle = ApplyGameplayEffectToOwner(
			Handle, 
			ActorInfo, 
			ActivationInfo, 
			SprintEffectClass.GetDefaultObject(), 
			GetAbilityLevel());
 	}
}

void UGA_Sprint::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
 	if (ActiveSprintEffectHandle.IsValid())
 	{
 		ActorInfo->AbilitySystemComponent->RemoveActiveGameplayEffect(ActiveSprintEffectHandle);
 	}
 	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Sprint::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	 const FGameplayAbilityActivationInfo ActivationInfo)
{
 	UE_LOG(LogAbilitySystem, Warning, TEXT("GA_Sprint :: Released"))
 	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
 	
 	EndAbility(CurrentSpecHandle, ActorInfo, ActivationInfo, true, false);
}