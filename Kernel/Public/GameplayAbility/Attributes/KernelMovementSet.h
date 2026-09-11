// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GameplayAbility/Attributes/KernelAttributeSet.h"
#include "KernelMovementSet.generated.h"

/**
 * 
 */
UCLASS()
class KERNEL_API UKernelMovementSet : public UKernelAttributeSet
{
	GENERATED_BODY()
	
public:
	UKernelMovementSet();
	
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	ATTRIBUTE_ACCESSORS(UKernelMovementSet, MoveSpeedMultiplier);
	
protected:
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MoveSpeedMultiplier, Category = "Movement")
	FGameplayAttributeData MoveSpeedMultiplier;
	
public:
	UFUNCTION()
	void OnRep_MoveSpeedMultiplier(const FGameplayAttributeData& OldValue);
};
	
	
