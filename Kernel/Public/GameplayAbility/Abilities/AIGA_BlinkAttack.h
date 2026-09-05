// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "AIGA_BlinkAttack.generated.h"

/**
 * 
 */
UCLASS()
class KERNEL_API UAIGA_BlinkAttack : public UGameplayAbility
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
		bool bReplicateEndAbility, bool bWasCancelled) override;
	
	/** 디졸브 아웃에 걸리는 시간 — Cue의 커브 길이와 맞출 것 */
	UPROPERTY(EditDefaultsOnly, Category = "Blink") float DissolveDuration = 0.45f;

	/** 경고가 뜬 뒤 실제 등장까지 — 플레이어의 반응 시간 */
	UPROPERTY(EditDefaultsOnly, Category = "Blink") float WarnBeforeAttack = 0.7f;

	UPROPERTY(EditDefaultsOnly, Category = "Blink") float AppearDuration = 0.35f;

	/** 플레이어로부터 이 거리 앞에 등장 */
	UPROPERTY(EditDefaultsOnly, Category = "Blink") float AppearDistance = 400.f;

	UPROPERTY(EditDefaultsOnly, Category = "Blink") FGameplayTag CueDisappear;
	UPROPERTY(EditDefaultsOnly, Category = "Blink") FGameplayTag CueAppear;
	UPROPERTY(EditDefaultsOnly, Category = "Blink") FGameplayTag AttackEventTag;
	
	UFUNCTION() void OnVanished();
	UFUNCTION() void OnWarned();
	UFUNCTION() void OnAppeared();
	
	bool FindAmbushLocation(AActor* Target, FVector& OutLocation) const;
	void SetVisualsEnabled(bool bEnabled);
	
	UPROPERTY(EditDefaultsOnly, Category = "Radius") FVector MeleeOffset = FVector(40,0,0);
	UPROPERTY(EditDefaultsOnly, Category = "Radius") float MeleeRadius = 200;
	
private:
	UPROPERTY() TObjectPtr<AActor> AmbushTarget = nullptr;
	UPROPERTY(EditDefaultsOnly) float Damage = 10.f;
	bool bVisualsDisabled = false;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> DamageEffectClass;
};
