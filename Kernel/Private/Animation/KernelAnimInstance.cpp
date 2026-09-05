// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/KernelAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void UKernelAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	
	OwningPawn = Cast<APawn>(TryGetPawnOwner());
}

void UKernelAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	if (!OwningPawn)
	{
		OwningPawn = Cast<APawn>(TryGetPawnOwner());
		if (!OwningPawn) return;
	}

	// GetControlRotation이 아니라 GetBaseAimRotation
	CachedAimRotation   = OwningPawn->GetBaseAimRotation();
	CachedActorRotation = OwningPawn->GetActorRotation();
}

void UKernelAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);
	
	
	if (ACharacter* OwnerCharacter = Cast<ACharacter>(TryGetPawnOwner()))
	{
		if (UCharacterMovementComponent* MovementComp = OwnerCharacter->GetCharacterMovement())
		{
			GroundSpeed = OwnerCharacter->GetVelocity().Size2D();
			Direction = CalculateDirection(OwnerCharacter->GetVelocity(), CachedAimRotation);
			bIsMoving = GroundSpeed > 0.0f;
			bIsFalling = MovementComp->IsFalling();
		}
	}
	
	/** Recoil */
	TargetRecoil = FMath::VInterpTo(TargetRecoil, FVector::ZeroVector, DeltaSeconds, 15.0f);
	CurrentRecoil = FMath::VInterpTo(CurrentRecoil, TargetRecoil, DeltaSeconds, 30.0f);
	
	
	
	/** AimOffset */
	const FRotator TargetDelta = (CachedAimRotation - CachedActorRotation).GetNormalized();

	SmoothedAimDelta = FMath::RInterpTo(SmoothedAimDelta, TargetDelta, DeltaSeconds, AimInterpSpeed);

	// 클램프는 보간 '후'에. 먼저 하면 경계에서 값이 튄다
	AimYaw   = FMath::Clamp(SmoothedAimDelta.Yaw,   YawClamp.X,   YawClamp.Y);
	AimPitch = FMath::Clamp(SmoothedAimDelta.Pitch, PitchClamp.X, PitchClamp.Y);

	AimOffsetAlpha = FMath::FInterpTo(AimOffsetAlpha, 1.f, DeltaSeconds, AlphaInterpSpeed);
}

void UKernelAnimInstance::AddRecoilKick(float KickAmount, float MaxRecoil)
{
	TargetRecoil.Y -= KickAmount;
	TargetRecoil.Y = FMath::Clamp(TargetRecoil.Y, -MaxRecoil, 0.0f);
}
