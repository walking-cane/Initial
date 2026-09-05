// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_Parry.generated.h"

/**
 * 
 */
UCLASS()
class KERNEL_API UGA_Parry : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGA_Parry();
	
protected:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, 
		const FGameplayEventData* TriggerEventData) override;
	
	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UAnimMontage> ParryMontage1P;
	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UAnimMontage> ParryMontage3P;
	UPROPERTY(EditDefaultsOnly)
	TSoftClassPtr<UGameplayEffect> ParryEffectClass;
	
	UFUNCTION() void OnMontageCompleted();
	UFUNCTION() void OnMontageCancelled();
};
