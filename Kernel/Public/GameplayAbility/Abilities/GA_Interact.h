// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_Interact.generated.h"

/**
 * 
 */
UCLASS()
class KERNEL_API UGA_Interact : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGA_Interact();
	
protected:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle, 
		const FGameplayAbilityActorInfo* ActorInfo, 
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;
};
