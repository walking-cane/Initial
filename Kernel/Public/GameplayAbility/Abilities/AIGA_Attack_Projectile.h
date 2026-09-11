// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "AIGA_Attack_Projectile.generated.h"

/**
 * 
 */
UCLASS()
class KERNEL_API UAIGA_Attack_Projectile : public UGameplayAbility
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
	
	bool InitFireInfo();
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAnimMontage> FireMontage;
	
	FTimerHandle FireTimerHandle;
};
