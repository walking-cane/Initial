// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbility/Attributes/KernelAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "KernelCombatAttributeSet.generated.h"

/**
 * 
 */
UCLASS()
class KERNEL_API UKernelCombatAttributeSet : public UKernelAttributeSet
{
	GENERATED_BODY()
	
public:
	UKernelCombatAttributeSet();
	
	ATTRIBUTE_ACCESSORS(UKernelCombatAttributeSet, BaseHeal);
	ATTRIBUTE_ACCESSORS(UKernelCombatAttributeSet, BaseDamage)
	ATTRIBUTE_ACCESSORS(UKernelCombatAttributeSet, MovementSpeed)
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	
protected:
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseDamage, Category = "Lyra|Combat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData BaseDamage;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseHeal, Category = "Lyra|Combat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData BaseHeal;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MovementSpeed, Category = "Lyra|Combat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MovementSpeed;
	
public:
	UFUNCTION()
	void OnRep_BaseDamage(const FGameplayAttributeData& OldValue);
	
	UFUNCTION()
	void OnRep_BaseHeal(const FGameplayAttributeData& OldValue);
	
	UFUNCTION()
	void OnRep_MovementSpeed(const FGameplayAttributeData& OldValue);
};
