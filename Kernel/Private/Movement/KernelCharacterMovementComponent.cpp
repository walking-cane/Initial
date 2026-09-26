// Fill out your copyright notice in the Description page of Project Settings.


#include "Movement/KernelCharacterMovementComponent.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/PhysicsVolume.h"
#include "GameplayAbility/KernelGameplayTags.h"
#include "GameplayAbility/Attributes/KernelCombatAttributeSet.h"
#include "GameplayAbility/Attributes/KernelMovementSet.h"
#include "KernelCharacter/Hero/KernelHeroCharacter.h"

void FSavedMove_Kernel::Clear()
{
	Super::Clear();
	bSavedWantsToClimb = false;
}

uint8 FSavedMove_Kernel::GetCompressedFlags() const
{
	uint8 Result = Super::GetCompressedFlags();
	if (bSavedWantsToClimb) { Result |= FLAG_Custom_0; }
	return Result;
}

bool FSavedMove_Kernel::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* Character, float MaxDelta) const
{
	const FSavedMove_Kernel* Other = static_cast<const FSavedMove_Kernel*>(NewMove.Get());
	if (bSavedWantsToClimb != Other->bSavedWantsToClimb) return false;   // 상태가 다르면 병합 금지
	return Super::CanCombineWith(NewMove, Character, MaxDelta);
}

void FSavedMove_Kernel::SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel,
								   FNetworkPredictionData_Client_Character& ClientData)
{
	Super::SetMoveFor(C, InDeltaTime, NewAccel, ClientData);
	if (const UKernelCharacterMovementComponent* CMC = Cast<UKernelCharacterMovementComponent>(C->GetCharacterMovement()))
	{
		bSavedWantsToClimb = CMC->bWantsToClimb;   // 저장
	}
}

void FSavedMove_Kernel::PrepMoveFor(ACharacter* C)
{
	Super::PrepMoveFor(C);
	if (UKernelCharacterMovementComponent* CMC = Cast<UKernelCharacterMovementComponent>(C->GetCharacterMovement()))
	{
		CMC->bWantsToClimb = bSavedWantsToClimb;   // 재생 시 복원
	}
}

FNetworkPredictionData_Client* UKernelCharacterMovementComponent::GetPredictionData_Client() const
{
	if (!ClientPredictionData)
	{
		UKernelCharacterMovementComponent* MutableThis = const_cast<UKernelCharacterMovementComponent*>(this);
		MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_Kernel(*this);
	}
	return ClientPredictionData;
}

void UKernelCharacterMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);
	bWantsToClimb = (Flags & FSavedMove_Character::FLAG_Custom_0) != 0;   // 서버가 받음
}

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
		0.f, 
		FColor::White, 
		FString::Printf(TEXT("Speed : %f"), Velocity.Size()));
	
	GEngine->AddOnScreenDebugMessage(1, 0.f, FColor::Yellow,
	   FString::Printf(TEXT("WantsClimb=%d Height=%.0f"), bWantsToClimb ? 1 : 0, GetHeightAboveGround()));
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

	const bool bWasSliding = 
		(PreviousMovementMode == MOVE_Custom && 
		(PreviousCustomMode == static_cast<uint8>(EKernelCustomMovementMode::Slide)));
	
	// for climbing
	const bool bWasClimbing = 
		(PreviousCustomMode == MOVE_Custom && 
		(PreviousCustomMode == static_cast<uint8>(EKernelCustomMovementMode::Climb) || 
		(PreviousCustomMode == static_cast<uint8>(EKernelCustomMovementMode::Mantle))));
	
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

	if (IsCustomMovementMode(EKernelCustomMovementMode::Slide))
	{
		const bool bFromSlideJump = (RetainedMomentum > 0.f);

		const float Now = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;
		const bool bOnCooldown = (Now - LastSlideEndTime) < SlideBoostCooldown;

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
	case EKernelCustomMovementMode::Slide:  PhysSlide(deltaTime, Iterations);  break;
	case EKernelCustomMovementMode::Climb:  PhysClimb(deltaTime, Iterations);  break;
	case EKernelCustomMovementMode::Mantle: PhysMantle(deltaTime, Iterations); break;
    
	default:
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

	if (!bWantsToCrouch) { return false; }

	if (GetPhysicsVolume() && GetPhysicsVolume()->bWaterVolume) { return false; }

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

void UKernelCharacterMovementComponent::UpdateCharacterStateBeforeMovement(float DeltaSeconds)
{
	Super::UpdateCharacterStateBeforeMovement(DeltaSeconds);
	
	if (!IsFalling() || !bWantsToClimb) return;
	if (GetHeightAboveGround() < MinClimbHeight) return;
    
	FHitResult Wall;
	if (!FindClimbWall(Wall)) return;

	ClimbElapsed = 0.f;
	SetMovementMode(MOVE_Custom, static_cast<uint8>(EKernelCustomMovementMode::Climb));
}

float UKernelCharacterMovementComponent::GetHeightAboveGround() const
{
	const float HalfH = CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	const FVector Feet = UpdatedComponent->GetComponentLocation() - FVector(0.f, 0.f, HalfH);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(CharacterOwner);

	FHitResult Hit;
	if (GetWorld()->LineTraceSingleByChannel(Hit, Feet, Feet - FVector(0.f, 0.f, GroundProbeDistance),
			ECC_Visibility, Params))
	{
		return Hit.Distance;
	}
	return GroundProbeDistance;
}

bool UKernelCharacterMovementComponent::FindClimbWall(FHitResult& OutHit) const
{
	UE_LOG(LogTemp,Warning,TEXT("Find Wall"))
	const float HalfH = CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	const FVector Fwd = UpdatedComponent->GetForwardVector();
	const FVector Start = UpdatedComponent->GetComponentLocation() + FVector(0.f, 0.f, HalfH * 0.3f);
	const FVector End = Start + Fwd * ClimbReach;
	
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(CharacterOwner);

	bool bTrace = GetWorld()->SweepSingleByChannel(OutHit, Start, End, FQuat::Identity,
			ECC_GameTraceChannel3, FCollisionShape::MakeSphere(20.f), Params);
	if (!bTrace)
	{
		return false;
	}
	
	/* Debug
	bTrace ? DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 3.f) :
			 DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 3.f);
	*/

	// 벽을 정면으로 보고 있을 때만
	return FVector::DotProduct(-OutHit.ImpactNormal, Fwd) > 0.7f;
}

bool UKernelCharacterMovementComponent::FindLedge(FVector& OutTop) const
{
	const float HalfH = CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	const FVector Fwd = UpdatedComponent->GetForwardVector();
	const FVector Head = UpdatedComponent->GetComponentLocation() + FVector(0.f, 0.f, HalfH);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(CharacterOwner);

	// 1) 머리 앞에 벽이 아직 있으면 → 아직 난간 아님
	FHitResult HeadHit;
	if (GetWorld()->LineTraceSingleByChannel(HeadHit, Head, Head + Fwd * ClimbReach, ECC_GameTraceChannel3, Params))
	{
		return false;
	}

	// 2) 벽 너머 위쪽에서 아래로 → 윗면 찾기
	const FVector DownStart = Head + Fwd * (ClimbReach + 20.f) + FVector(0.f, 0.f, 60.f);
	const FVector DownEnd   = DownStart - FVector(0.f, 0.f, 140.f);

	FHitResult TopHit;
	if (!GetWorld()->LineTraceSingleByChannel(TopHit, DownStart, DownEnd, ECC_Visibility, Params))
	{
		return false;
	}

	if (TopHit.ImpactNormal.Z < 0.7f) return false;   // 경사가 심하면 못 올라섬

	OutTop = TopHit.ImpactPoint;
	return true;
}

bool UKernelCharacterMovementComponent::HasRoomToStand(const FVector& Top) const
{
	const UCapsuleComponent* Cap = CharacterOwner->GetCapsuleComponent();
	const FVector Center = Top + FVector(0.f, 0.f, Cap->GetScaledCapsuleHalfHeight() + 2.f);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(CharacterOwner);

	return !GetWorld()->OverlapAnyTestByChannel(
		Center, FQuat::Identity, ECC_Pawn, Cap->GetCollisionShape(), Params);
}

void UKernelCharacterMovementComponent::PhysClimb(float DeltaTime, int32 Iterations)
{
	if (DeltaTime < MIN_TICK_TIME) return;

	ClimbElapsed += DeltaTime;

	const bool bHeld = bWantsToClimb;
	if (!bHeld || ClimbElapsed > MaxClimbTime)
	{
		SetMovementMode(MOVE_Falling);
		StartNewPhysics(DeltaTime, Iterations);
		return;
	}

	// 난간에 도달 → 올라서기
	FVector LedgeTop;
	if (FindLedge(LedgeTop) && HasRoomToStand(LedgeTop))
	{
		StartMantle(LedgeTop);
		return;
	}

	// 벽이 사라짐 → 낙하
	FHitResult Wall;
	if (!FindClimbWall(Wall))
	{
		SetMovementMode(MOVE_Falling);
		StartNewPhysics(DeltaTime, Iterations);
		return;
	}

	// 위로 이동 + 벽 쪽으로 붙이기
	Velocity = FVector::UpVector * ClimbSpeed;

	const FVector ToWall = -Wall.ImpactNormal * (Wall.Distance - WallStickDistance);
	const FVector Delta  = Velocity * DeltaTime + ToWall;
	const FRotator FaceWall = (-Wall.ImpactNormal).Rotation();

	FHitResult Hit;
	SafeMoveUpdatedComponent(Delta, FaceWall, true, Hit);
	if (Hit.IsValidBlockingHit())
	{
		SlideAlongSurface(Delta, 1.f - Hit.Time, Hit.Normal, Hit, true);
	}
}

void UKernelCharacterMovementComponent::PhysMantle(float DeltaTime, int32 Iterations)
{
	MantleElapsed += DeltaTime;
	const float Alpha = FMath::Clamp(MantleElapsed / MantleDuration, 0.f, 1.f);

	// 위로 먼저 올리고 앞으로 넘어가는 느낌 — Z를 빨리, XY를 나중에
	const float ZAlpha  = FMath::InterpEaseOut(0.f, 1.f, Alpha, 2.f);
	const float XYAlpha = FMath::InterpEaseIn(0.f, 1.f, Alpha, 2.f);

	FVector NewLoc;
	NewLoc.X = FMath::Lerp(MantleStart.X, MantleTarget.X, XYAlpha);
	NewLoc.Y = FMath::Lerp(MantleStart.Y, MantleTarget.Y, XYAlpha);
	NewLoc.Z = FMath::Lerp(MantleStart.Z, MantleTarget.Z, ZAlpha);

	FHitResult Hit;
	SafeMoveUpdatedComponent(NewLoc - UpdatedComponent->GetComponentLocation(),
		UpdatedComponent->GetComponentQuat(), false, Hit);

	if (Alpha >= 1.f)
	{
		SetMovementMode(MOVE_Walking);
	}
}

void UKernelCharacterMovementComponent::StartMantle(const FVector& Top)
{ 
	const float HalfH = CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	MantleStart  = UpdatedComponent->GetComponentLocation();
	MantleTarget = Top + FVector(0.f, 0.f, HalfH + 2.f);
	MantleElapsed = 0.f;
	Velocity = FVector::ZeroVector;

	SetMovementMode(MOVE_Custom, static_cast<uint8>(EKernelCustomMovementMode::Mantle));
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