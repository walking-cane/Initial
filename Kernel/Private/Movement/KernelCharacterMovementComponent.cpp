// Fill out your copyright notice in the Description page of Project Settings.


#include "Movement/KernelCharacterMovementComponent.h"
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
