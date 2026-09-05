// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GameplayAbility/Attributes/KernelAttributeSet.h"
#include "KernelHealthAttributeSet.generated.h"

/**
 * 
 */
UCLASS()
class KERNEL_API UKernelHealthAttributeSet : public UKernelAttributeSet
{
	GENERATED_BODY()
	
public:
	UKernelHealthAttributeSet();
	
	ATTRIBUTE_ACCESSORS(UKernelHealthAttributeSet, Health);
	ATTRIBUTE_ACCESSORS(UKernelHealthAttributeSet, MaxHealth);
	
	ATTRIBUTE_ACCESSORS(UKernelHealthAttributeSet, Shield);
	ATTRIBUTE_ACCESSORS(UKernelHealthAttributeSet, MaxShield);
	
	ATTRIBUTE_ACCESSORS(UKernelHealthAttributeSet, Healing);
	
	ATTRIBUTE_ACCESSORS(UKernelHealthAttributeSet, Armor)
	ATTRIBUTE_ACCESSORS(UKernelHealthAttributeSet, Damage)
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual bool PreGameplayEffectExecute(struct FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	
	mutable FKernelAttributeEvent OnHealthChanged;
	mutable FKernelAttributeEvent OnMaxHealthChanged;
	mutable FKernelAttributeEvent OnOutOfHealth;
	
protected:
	//HP system
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category="Attribute | Health")
	FGameplayAttributeData Health;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category="Attribute | Health")
	FGameplayAttributeData MaxHealth = 100.f;
	
	UPROPERTY(BlueprintReadOnly, Category="Attribute | Health")
	FGameplayAttributeData Shield;
	UPROPERTY(BlueprintReadOnly, Category="Attribute | Health")
	FGameplayAttributeData MaxShield = 50.f;
	
	UPROPERTY(BlueprintReadOnly, Category="Attribute | Health")
	FGameplayAttributeData Healing;
	
	UPROPERTY(BlueprintReadOnly, Category = "Armor")
	FGameplayAttributeData Armor;
	UPROPERTY(BlueprintReadOnly, Category = "Armor")
	FGameplayAttributeData Damage;
	
public:
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldValue);
	
private:
	bool bOutOfHealth;
	
	// Caching before change
	float HealthBeforeAttributeChange;
	float MaxHealthBeforeAttributeChange;
};
