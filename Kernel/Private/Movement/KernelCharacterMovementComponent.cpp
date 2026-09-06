// Fill out your copyright notice in the Description page of Project Settings.


#include "Movement/KernelCharacterMovementComponent.h"

#include "GameFramework/Character.h"
#include "GameplayAbility/Attributes/KernelCombatAttributeSet.h"

void UKernelCharacterMovementComponent::InitializeASC(UAbilitySystemComponent* InASC)
{
	CachedASC = InASC;
}

float UKernelCharacterMovementComponent::GetMaxSpeed() const
{
	float BaseSpeed = Super::GetMaxSpeed();
	
	if (CachedASC)
	{
		float CustomSpeed = CachedASC->GetNumericAttribute(UKernelCombatAttributeSet::GetMovementSpeedAttribute());
		FVector InputDir = GetCurrentAcceleration().GetSafeNormal();
		
		if (!InputDir.IsZero())
		{
			FVector ForwardDir = GetOwner()->GetActorForwardVector();
			float DotProduct = FVector::DotProduct(ForwardDir, InputDir);
			
			if (DotProduct >= 0.8f)
			{
				return CustomSpeed;
			}
			else
			{
				return BaseSpeed; 
			}
		}
		// No Input (No acceleration)
		return CustomSpeed;
	}
	// Failed Caching ASC
	return BaseSpeed;
}

void UKernelCharacterMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode,
	uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);
	
	if (IsCustomMovementMode(EKernelCustomMovementMode::Slide))
	{
		Crouch();
	}
}

void UKernelCharacterMovementComponent::PhysCustom(float deltaTime, int32 Iterations)
{
	Super::PhysCustom(deltaTime, Iterations);
	
	switch (static_cast<EKernelCustomMovementMode>(CustomMovementMode))
	{
	case EKernelCustomMovementMode::Slide:
		PhysSlide(deltaTime, Iterations);
		break;

	default:
		// 모르는 모드로 들어오면 안전하게 복귀
		SetMovementMode(MOVE_Walking);
		break;
	}
}

void UKernelCharacterMovementComponent::PhysSlide(float deltaTime, int32 Iterations)
{
	if (deltaTime < MIN_TICK_TIME) { return; }
	
	if (!CharacterOwner || (!CharacterOwner->Controller && !bRunPhysicsWithNoController
		&& !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity()
		&& CharacterOwner->GetLocalRole() != ROLE_SimulatedProxy))
	{
		Acceleration = FVector::ZeroVector;
		Velocity = FVector::ZeroVector;
		return;
	}
	
	if (!UpdatedComponent->IsQueryCollisionEnabled())
	{
		SetMovementMode(MOVE_Walking);
		return;
	}
	
	FFindFloorResult FloorResult;
	FindFloor(UpdatedComponent->GetComponentLocation(), FloorResult, false);
	
	UE_LOG(LogTemp,Log,TEXT("PhysSlide GGGG"))
	
	if (!FloorResult.IsWalkableFloor())
	{
		SetMovementMode(MOVE_Falling);
		StartNewPhysics(deltaTime, Iterations);
		return;
	}
}
