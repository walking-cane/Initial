// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "KernelCustomMovementMode.h"
#include "KernelCharacterMovementComponent.generated.h"

class UAbilitySystemComponent;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnSlideStateChanged, bool /*bIsSliding*/);

UCLASS()
class UKernelCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
public:
	void InitializeASC(UAbilitySystemComponent* InASC);
	
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	virtual float GetMaxSpeed() const override;
	virtual bool IsMovingOnGround() const override;
	virtual bool CanAttemptJump() const override;
	virtual bool CanCrouchInCurrentState() const override;
	virtual float GetMaxBrakingDeceleration() const override;
	
	virtual void Crouch(bool bClientSimulation = false) override;
	virtual void UnCrouch(bool bClientSimulation = false) override;
	
	bool IsCustomMovementMode(EKernelCustomMovementMode CustomMode) const
	{
		return MovementMode == MOVE_Custom && CustomMovementMode == static_cast<uint8>(CustomMode);
	}
	void SetCustomMovementMode(EKernelCustomMovementMode CustomMode)
	{
		SetMovementMode(MOVE_Custom, static_cast<uint8>(CustomMode));
	}
	
	FOnSlideStateChanged OnSlideStateChanged;
	
protected:
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;
	virtual void SetPostLandedPhysics(const FHitResult& Hit) override;
	
	virtual void PhysCustom(float deltaTime, int32 Iterations) override;
	void PhysSlide(float deltaTime, int32 Iterations);

	bool CanStartSlideOnLanded() const;
	
	//Slide & SlideJump
	UPROPERTY(EditDefaultsOnly, Category = "Slide") 
	float SlideBoost = 1.2f; // 슬라이드 시작 시 속도 배율
	UPROPERTY(EditDefaultsOnly, Category = "Slide") 
	float SlideSlopeForce = 1.2f; // 내리막 속도 가속 배율
	UPROPERTY(EditDefaultsOnly, Category = "Slide") 
	float SlideMinSpeed = 300.f; // 슬라이딩 진입 최소 속도 (미만일 시 Crouch)
	UPROPERTY(EditDefaultsOnly, Category = "Slide")
	float SlideExitSpeed = 180.f;
	UPROPERTY(EditDefaultsOnly, Category = "Slide") 
	float SlideMaxSpeed = 1500.f; // 슬라이딩 최고 속도
	UPROPERTY(EditDefaultsOnly, Category = "Slide") 
	float SlideFriction = 0.06f; // 슬라이딩 종료 시 마찰 제동
	UPROPERTY(EditDefaultsOnly, Category = "Slide") 
	float SlideControlValue = 200.f; // 슬라이딩 시 조향 정도
	UPROPERTY(EditDefaultsOnly, Category = "Slide") 
	float SlideBrakingDeceleration = 300.f; // 슬라이딩 시 마찰 제동
	UPROPERTY(EditDefaultsOnly, Category = "Slide") 
	float SlideSteerRate = 90.f;   // 초당 회전 각도(도). 0이면 조향 불가
	UPROPERTY(EditDefaultsOnly, Category = "Slide") 
	float SlideJumpBoost = 1.1f;        // 슬점 시 수평 가산
	UPROPERTY(EditDefaultsOnly, Category = "Slide") 
	float MomentumDecayRate = 400.f;    // 슬점 초당 운동량 감쇠 (cm/s)
	UPROPERTY(EditDefaultsOnly, Category = "Slide")
	float SlideLandEnterSpeed = 400.f;   // 착지 슬라이드 진입에 필요한 최소 수평 속도
	UPROPERTY(EditDefaultsOnly, Category = "Slide")
	float SlideBoostCooldown = 0.8f;   // 이 시간 안에 재진입하면 부스트 없음
	
	float RetainedMomentum = 0.f;
	
private:
	float LastSlideEndTime = -1000.f;
	
	UPROPERTY() UAbilitySystemComponent* CachedASC;
	
	void CompensateCameraForRootMove(float PreRootZ);
	
	FTimerHandle ASCTimerHandle;
};
