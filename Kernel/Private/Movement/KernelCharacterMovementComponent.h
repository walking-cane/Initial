// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "KernelCustomMovementMode.h"
#include "KernelCharacterMovementComponent.generated.h"

class UAbilitySystemComponent;

UCLASS()
class UKernelCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
public:
	void InitializeASC(UAbilitySystemComponent* InASC);
	virtual float GetMaxSpeed() const override;
	
protected:
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;
	virtual void PhysCustom(float deltaTime, int32 Iterations) override;
	
	void PhysSlide(float deltaTime, int32 Iterations);
	
	bool IsCustomMovementMode(EKernelCustomMovementMode CustomMode)
	{
		return MovementMode == MOVE_Custom && CustomMovementMode == static_cast<uint8>(CustomMode);
	}
	void SetCustomMovementMode(EKernelCustomMovementMode CustomMode)
	{
		SetMovementMode(MOVE_Custom, static_cast<uint8>(CustomMode));
	}
private:
	UPROPERTY() UAbilitySystemComponent* CachedASC;
	
	FTimerHandle ASCTimerHandle;
};
