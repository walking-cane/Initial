// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_EquipWeapon.generated.h"

class UKernelItemFragment_Cosmetic;
class UKernelItemFragment_Weapon;
class UKernelItemFragment_Equippable;

UCLASS()
class KERNEL_API UGA_EquipWeapon : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGA_EquipWeapon();
	
protected:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;
	
	UPROPERTY() 
	TObjectPtr<const AActor> Player;
	
	// 기존 WeaponData를 삭제하고, 장착 관련 데이터를 쥐고 있는 Fragment로 대체합니다.
	UPROPERTY() 
	TObjectPtr<const UKernelItemFragment_Cosmetic> CosmeticFrag;
	UPROPERTY() 
	TObjectPtr<const UKernelItemFragment_Weapon> WeaponFrag;
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Equip")
	TSubclassOf<UGameplayEffect> EquipLockEffect;
};