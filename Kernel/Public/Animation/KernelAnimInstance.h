// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "KernelAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class KERNEL_API UKernelAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
	
public:
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
protected:
	UPROPERTY(BlueprintReadOnly) float GroundSpeed;
	UPROPERTY(BlueprintReadOnly) float Direction;
	
	UPROPERTY(BlueprintReadOnly) bool bIsMoving;
	UPROPERTY(BlueprintReadOnly) bool bIsFalling;
	UPROPERTY(BlueprintReadOnly) float AimYaw = 0.f;
	UPROPERTY(BlueprintReadOnly) float AimPitch = 0.f;
	UPROPERTY(BlueprintReadOnly) float AimOffsetAlpha = 0.f;
	UPROPERTY(BlueprintReadOnly) float AimInterpSpeed = 15.f;
	UPROPERTY(BlueprintReadOnly) float AlphaInterpSpeed = 8.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "AimOffset")
	FVector2D YawClamp = FVector2D(-90.f, 90.f);

	UPROPERTY(EditDefaultsOnly, Category = "AimOffset")
	FVector2D PitchClamp = FVector2D(-90.f, 90.f);
	
	UPROPERTY(BlueprintReadOnly) FVector CurrentRecoil;
	FVector TargetRecoil;
	
public:
	UFUNCTION(BlueprintCallable, Category = "Recoil")
	void AddRecoilKick(float KickAmount, float MaxRecoil);
	
private:
	UPROPERTY(Transient)
	TObjectPtr<APawn> OwningPawn = nullptr;
	
	FRotator CachedAimRotation   = FRotator::ZeroRotator;
	FRotator CachedActorRotation = FRotator::ZeroRotator;

	FRotator SmoothedAimDelta = FRotator::ZeroRotator;
};
