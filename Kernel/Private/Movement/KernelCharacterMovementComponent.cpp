// Fill out your copyright notice in the Description page of Project Settings.


#include "Movement/KernelCharacterMovementComponent.h"

#include "GameFramework/Character.h"
#include "GameFramework/PhysicsVolume.h"
#include "GameplayAbility/KernelGameplayTags.h"
#include "GameplayAbility/Attributes/KernelCombatAttributeSet.h"
#include "GameplayAbility/Attributes/KernelMovementSet.h"
#include "KernelCharacter/Hero/KernelHeroCharacter.h"

void UKernelCharacterMovementComponent::InitializeASC(UAbilitySystemComponent* InASC)
{
	CachedASC = InASC;
}

void UKernelCharacterMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// For Debug
	GEngine->AddOnScreenDebugMessage(
		-1, 
		-1, 
		FColor::White, 
		FString::Printf(TEXT("Speed : %f"), Velocity.Size()));
}

float UKernelCharacterMovementComponent::GetMaxSpeed() const
{
	// 사망중에는 이동불가
	if (CachedASC && CachedASC->HasMatchingGameplayTag(TAG_Status_Death_Dying))
	{
		return 0.f;
	}

	// ── 2단계: 이 상태의 "기준 속도"를 정한다
	float Speed = 0.f;
	bool bApplyModifiers = true;

	if (IsCustomMovementMode(EKernelCustomMovementMode::Slide))
	{
		Speed = SlideMaxSpeed;
		bApplyModifiers = false;    // 슬라이드는 운동량 기반
	}
	else if (MovementMode == MOVE_Falling && RetainedMomentum > 0.f)
	{
		// 이미 확정된 운동량. 사후에 무기 배율로 깎으면 공중에서 감속 — 그대로 둠
		return FMath::Max(RetainedMomentum, Super::GetMaxSpeed());
	}
	else if (IsCrouching() && IsMovingOnGround())
	{
		Speed = MaxWalkSpeedCrouched;   // 절대속도 어트리뷰트는 무시 (기존 의도 유지)
	}
	else
	{
		bool bFound = false;
		const float Custom = CachedASC
			? CachedASC->GetGameplayAttributeValue(
				  UKernelCombatAttributeSet::GetMovementSpeedAttribute(), bFound)
			: 0.f;

		Speed = bFound ? Custom : Super::GetMaxSpeed();   // [변경] 미부여 시 0 대신 폴백
	}

	if (!bApplyModifiers)
	{
		return Speed;
	}
	 
	// ── 4단계: 무기/버프 배율. 여기가 유일한 적용 지점
	if (CachedASC)
	{
		bool bFound = false;
		const float Mult = CachedASC->GetGameplayAttributeValue(
			UKernelMovementSet::GetMoveSpeedMultiplierAttribute(), bFound);
		if (bFound)
		{
			Speed *= Mult;
		}
	}

	return Speed;
}

void UKernelCharacterMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode,
	uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);

	const bool bWasSliding = (PreviousMovementMode == MOVE_Custom
		&& PreviousCustomMode == static_cast<uint8>(EKernelCustomMovementMode::Slide));

	// ===== 슬라이드 이탈 =====
	if (bWasSliding)
	{
		bOrientRotationToMovement = true;
		LastSlideEndTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;   // 추가
		OnSlideStateChanged.Broadcast(false);
		
		// 점프로 나가는 경우에만 운동량을 공중으로 넘긴다
		if (MovementMode == MOVE_Falling)
		{
			const float ExitSpeed = FMath::Min(Velocity.Size2D() * SlideJumpBoost, SlideMaxSpeed);
			RetainedMomentum = ExitSpeed;

			const FVector Dir = Velocity.GetSafeNormal2D();
			Velocity.X = Dir.X * ExitSpeed;
			Velocity.Y = Dir.Y * ExitSpeed;
		}
	}

	// ===== 슬라이드 진입 =====
	if (IsCustomMovementMode(EKernelCustomMovementMode::Slide))
	{
		const bool bFromSlideJump = (RetainedMomentum > 0.f);

		const float Now = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;
		const bool bOnCooldown = (Now - LastSlideEndTime) < SlideBoostCooldown;

		// 슬라이드 점프 착지거나 쿨다운 중이면 현재 속도를 그대로 쓴다
		const bool bAllowBoost = !bFromSlideJump && !bOnCooldown;

		bCrouchMaintainsBaseLocation = true;
		SetGroundMovementMode(MOVE_Walking);

		FindFloor(UpdatedComponent->GetComponentLocation(), CurrentFloor, false);
		AdjustFloorHeight();
		SetBaseFromFloor(CurrentFloor);

		bOrientRotationToMovement = false;

		const FVector Dir = Velocity.GetSafeNormal2D();
		const float CurSpeed = Velocity.Size2D();

		const float EntrySpeed = bAllowBoost
			? FMath::Clamp(CurSpeed * SlideBoost, SlideMinSpeed, SlideMaxSpeed)
			: FMath::Min(CurSpeed, SlideMaxSpeed);

		Velocity = Dir * EntrySpeed;

		RetainedMomentum = 0.f;
		OnSlideStateChanged.Broadcast(true);
	}

	// ===== 걷기 착지 =====
	if (MovementMode == MOVE_Walking)
	{
		RetainedMomentum = 0.f;
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

bool UKernelCharacterMovementComponent::CanAttemptJump() const
{
	return IsJumpAllowed() && (IsMovingOnGround() || IsFalling());
}

bool UKernelCharacterMovementComponent::CanCrouchInCurrentState() const
{
	// 공중에서는 캡슐을 펴둔다. bWantsToCrouch는 그대로 유지되므로
	// 착지하면 다시 앉고, 속도가 충분하면 슬라이드로 이어진다.
	if (IsFalling())
	{
		return false;
	}
	return Super::CanCrouchInCurrentState();
}

bool UKernelCharacterMovementComponent::IsMovingOnGround() const
{
	return Super::IsMovingOnGround() || IsCustomMovementMode(EKernelCustomMovementMode::Slide);
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

	if (!FloorResult.IsWalkableFloor())
	{
		SetMovementMode(MOVE_Falling);
		StartNewPhysics(deltaTime, Iterations);
		return;
	}
	
	CurrentFloor = FloorResult;

	// 경사 가속
	const FVector SlopeDir = FVector::VectorPlaneProject(
		FVector::DownVector, FloorResult.HitResult.ImpactNormal).GetSafeNormal();

	Velocity += SlopeDir * SlideSlopeForce * deltaTime;

	// 상한 클램프. 조건과 대입값이 반드시 같은 값이어야 한다.
	if (Velocity.SizeSquared() > FMath::Square(SlideMaxSpeed))
	{
		Velocity = Velocity.GetSafeNormal() * SlideMaxSpeed;
	}

	const FVector InputDir = Acceleration.GetSafeNormal2D();
	Acceleration = FVector::ZeroVector;

	if (!InputDir.IsNearlyZero() && SlideSteerRate > 0.f)
	{
		const float Speed = Velocity.Size();
		const FVector CurDir = Velocity.GetSafeNormal();
		const FVector NewDir = FMath::VInterpNormalRotationTo(CurDir, InputDir, deltaTime, SlideSteerRate);

		Velocity = NewDir * Speed;   // 크기 불변 → 가속 불가능
	}

	if (!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
	{
		CalcVelocity(deltaTime, SlideFriction, false, GetMaxBrakingDeceleration());
	}
	ApplyRootMotionToVelocity(deltaTime);

	// 수직 성분 제거 — 바닥을 따라 미끄러진다
	Velocity = FVector::VectorPlaneProject(Velocity, FloorResult.HitResult.ImpactNormal);

	Iterations++;
	bJustTeleported = false;

	const FVector OldLocation = UpdatedComponent->GetComponentLocation();
	const FVector Delta = Velocity * deltaTime;
	FHitResult Hit(1.f);

	SafeMoveUpdatedComponent(Delta, UpdatedComponent->GetComponentQuat(), true, Hit);

	if (Hit.Time < 1.f)
	{
		HandleImpact(Hit, deltaTime, Delta);
		SlideAlongSurface(Delta, 1.f - Hit.Time, Hit.Normal, Hit, true);
	}

	// --- 이동 후 바닥 재판정 & 스냅 ---
	FFindFloorResult NewFloor;
	FindFloor(UpdatedComponent->GetComponentLocation(), NewFloor, false);

	if (NewFloor.IsWalkableFloor())
	{
		CurrentFloor = NewFloor;
		AdjustFloorHeight();               // 캡슐을 바닥에 정확히 붙인다
		SetBaseFromFloor(NewFloor);        // 무빙 플랫폼 대응
	}
	else
	{
		SetMovementMode(MOVE_Falling);
		StartNewPhysics(deltaTime, Iterations);
		return;
	}

	// --- 실제 이동량으로 속도 보정 ---
	if (!bJustTeleported && !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
	{
		Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / deltaTime;
		MaintainHorizontalGroundVelocity();
	}

	// --- 종료 조건 ---
	if (Velocity.SizeSquared() < FMath::Square(SlideExitSpeed))
	{
		SetMovementMode(MOVE_Walking);
		StartNewPhysics(deltaTime, Iterations);
	}
}

bool UKernelCharacterMovementComponent::CanStartSlideOnLanded() const
{
	if (!CharacterOwner) { return false; }

	// 앉기 키를 누르고 있다는 신호. GA_Crouch가 Char->Crouch()로 세워둔 값이다.
	if (!bWantsToCrouch) { return false; }

	// 물속 착지는 엔진 기본 처리(수영)에 맡긴다
	if (GetPhysicsVolume() && GetPhysicsVolume()->bWaterVolume) { return false; }

	// 수평 속도만 본다. 낙하 Z속도로 슬라이드가 발동되면 안 된다.
	return Velocity.Size2D() >= SlideLandEnterSpeed;
}

void UKernelCharacterMovementComponent::SetPostLandedPhysics(const FHitResult& Hit)
{
	if (CanStartSlideOnLanded())
	{
		SetCustomMovementMode(EKernelCustomMovementMode::Slide);
		return;
	}

	Super::SetPostLandedPhysics(Hit);
}

float UKernelCharacterMovementComponent::GetMaxBrakingDeceleration() const
{
	if (IsCustomMovementMode(EKernelCustomMovementMode::Slide))
	{
		return SlideBrakingDeceleration;
	}
	return Super::GetMaxBrakingDeceleration();
}

void UKernelCharacterMovementComponent::Crouch(bool bClientSimulation)
{
	const float PreZ = UpdatedComponent ? UpdatedComponent->GetComponentLocation().Z : 0.f;
	Super::Crouch(bClientSimulation);
	CompensateCameraForRootMove(PreZ);
}

void UKernelCharacterMovementComponent::UnCrouch(bool bClientSimulation)
{
	const float PreZ = UpdatedComponent ? UpdatedComponent->GetComponentLocation().Z : 0.f;
	Super::UnCrouch(bClientSimulation);
	CompensateCameraForRootMove(PreZ);
}

void UKernelCharacterMovementComponent::CompensateCameraForRootMove(float PreRootZ)
{
	if (!UpdatedComponent || !CharacterOwner || !CharacterOwner->IsLocallyControlled())
	{
		return;
	}

	const float DeltaZ = UpdatedComponent->GetComponentLocation().Z - PreRootZ;
	if (FMath::IsNearlyZero(DeltaZ, 0.01f))
	{
		return;   // 루트가 안 움직였으면 보상할 것도 없다
	}

	if (AKernelHeroCharacter* Hero = Cast<AKernelHeroCharacter>(CharacterOwner))
	{
		// 루트가 내려간 만큼 카메라를 올려 월드 위치를 유지한다
		Hero->AddCameraHeightCompensation(-DeltaZ);
	}
}