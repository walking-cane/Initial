// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_Sprint.generated.h"

/**
 * 
 */
UCLASS()
class KERNEL_API UGA_Sprint : public UGameplayAbility
{
	GENERATED_BODY()
	
protected:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle, 
		const FGameplayAbilityActorInfo* ActorInfo, 
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;
	
	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility, 
		bool bWasCancelled) override;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> SprintEffectClass;
	FActiveGameplayEffectHandle ActiveSprintEffectHandle;
	
public:
	virtual void InputReleased(
	const FGameplayAbilitySpecHandle Handle, 
	const FGameplayAbilityActorInfo* ActorInfo, 
	const FGameplayAbilityActivationInfo ActivationInfo) override;
};