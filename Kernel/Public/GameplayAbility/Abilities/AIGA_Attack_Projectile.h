// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "AIGA_Attack_Projectile.generated.h"

class AKernelProjectileBase;
/**
 * 
 */
UCLASS()
class KERNEL_API UAIGA_Attack_Projectile : public UGameplayAbility
{
	GENERATED_BODY()
	
	UAIGA_Attack_Projectile();
	
protected:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo, 
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;
	
	bool InitFireInfo();
	
	FRotator ApplySpread(const FRotator& BaseRot) const;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> FireMontage;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AKernelProjectileBase> ProjectileClass;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> DamageEffectClass;
	UPROPERTY(EditDefaultsOnly)
	float Damage = 10.f;
	UPROPERTY(EditDefaultsOnly)
	float ProjectileSpeed = 500.f;
	UPROPERTY(EditDefaultsOnly)
	float SpreadHalfAngle = 3.f;

	
private:
	UFUNCTION() void OnMontageCancelled();
	UFUNCTION() void OnMontageCompleted();
	
	UFUNCTION() void OnEventReceived(FGameplayEventData Payload);
	
	FVector Loc;
	FRotator Rot;
	
	UPROPERTY() TObjectPtr<const AActor> CachedTarget = nullptr;
};
