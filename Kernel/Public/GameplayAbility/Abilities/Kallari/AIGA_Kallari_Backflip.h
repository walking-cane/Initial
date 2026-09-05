// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "AIGA_Kallari_Backflip.generated.h"

/**
 * 
 */
UCLASS()
class KERNEL_API UAIGA_Kallari_Backflip : public UGameplayAbility
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
	TObjectPtr<UAnimMontage> BackflipMontage;

private:
	UPROPERTY(EditDefaultsOnly)
	float BackSpeed = 800.f;
	UPROPERTY(EditDefaultsOnly)
	float UpSpeed = 600.f;
	
	UFUNCTION() void OnMontageEnded();
	UFUNCTION() void OnLanded(const FHitResult& Hit);
};
