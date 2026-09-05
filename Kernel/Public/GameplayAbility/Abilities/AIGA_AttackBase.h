// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "AIGA_AttackBase.generated.h"

UCLASS()
class KERNEL_API UAIGA_AttackBase : public UGameplayAbility
{
	GENERATED_BODY()
	
protected:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, 
		const FGameplayEventData* TriggerEventData) override;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> AttackMontage;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> DamageEffectClass;
	UPROPERTY(EditDefaultsOnly)
	float AttackRadius;
	UPROPERTY(EditDefaultsOnly)
	float AttackDistance;
	UPROPERTY(EditDefaultsOnly)
	float Damage;
	
	UFUNCTION()
	void OnMontageCompleted();
	UFUNCTION()
	void OnMontageCancelled();
	
	UFUNCTION()
	void OnHitEventReceived(FGameplayEventData Payload);
};
