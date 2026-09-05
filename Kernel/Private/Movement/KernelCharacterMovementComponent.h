// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "KernelCharacterMovementComponent.generated.h"

class UAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class UKernelCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
public:
	void InitializeASC(UAbilitySystemComponent* InASC);
	virtual float GetMaxSpeed() const override;
	
private:
	UPROPERTY() UAbilitySystemComponent* CachedASC;
	
	FTimerHandle ASCTimerHandle;
};
