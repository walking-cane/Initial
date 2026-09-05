// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbility/Abilities/AIGA_BlinkAttack.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "NavigationSystem.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "KernelCharacter/KernelPlayerController.h"
#include "KernelCharacter/Enemy/KernelEnemyCharacter.h"

void UAIGA_BlinkAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                        const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                        const FGameplayEventData* TriggerEventData)
{
	UE_LOG(LogTemp,Warning,TEXT("[AIGA_BlinkAttack] Activate"))
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	ACharacter* Avatar = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	AmbushTarget = TriggerEventData ? const_cast<AActor*>(TriggerEventData->Target.Get()) : nullptr;
	
	if (!Avatar || !AmbushTarget)
	{
		UE_LOG(LogTemp,Error,TEXT("[AIGA_BlinkAttack] No Target"))
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	FVector DummyLocation;
	if (!FindAmbushLocation(AmbushTarget, DummyLocation))
	{
		UE_LOG(LogTemp,Error,TEXT("[AIGA_BlinkAttack] No Location"))
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	if (UCharacterMovementComponent* CMC = Avatar->GetCharacterMovement())
	{
		CMC->StopMovementImmediately();
		CMC->SetMovementMode(MOVE_None);
	}
	Avatar->SetActorEnableCollision(false);
	bVisualsDisabled = true;
	
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		FGameplayCueParameters P;
		P.Location = Avatar->GetActorLocation();
		P.RawMagnitude = DissolveDuration;   // Cue가 디졸브 속도를 맞추도록
		
		ASC->ExecuteGameplayCue(CueDisappear, P);
	}
	UE_LOG(LogTemp, Warning, TEXT("[Blink] DissolveDuration=%.3f"), DissolveDuration);

	UAbilityTask_WaitDelay* Task = UAbilityTask_WaitDelay::WaitDelay(this, DissolveDuration);
	Task->OnFinish.AddDynamic(this, &ThisClass::OnVanished);
	Task->ReadyForActivation();
}

void UAIGA_BlinkAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	SetVisualsEnabled(true);
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UAIGA_BlinkAttack::OnVanished()
{
	UE_LOG(LogTemp,Warning,TEXT("[AIGA_BlinkAttack] Vanished"))
	ACharacter* Avatar = Cast<ACharacter>(GetAvatarActorFromActorInfo());

	if (!Avatar || !IsValid(AmbushTarget))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}
	
	// 표적 1명에게만 경고
	if (APawn* TargetPawn = Cast<APawn>(AmbushTarget))
	{
		if (AKernelPlayerController* PC = Cast<AKernelPlayerController>(TargetPawn->GetController()))
		{
			PC->Client_ShowAttackWarning(FVector(0,0,0), WarnBeforeAttack);
		}
	}
	
	UAbilityTask_WaitDelay* Task = UAbilityTask_WaitDelay::WaitDelay(this, WarnBeforeAttack);
	Task->OnFinish.AddDynamic(this, &ThisClass::OnWarned);
	Task->ReadyForActivation();
}

void UAIGA_BlinkAttack::OnWarned()
{
	ACharacter* Avatar = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Avatar)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}
	
	FVector Dest;
	if (!FindAmbushLocation(AmbushTarget, Dest))
	{
		UE_LOG(LogTemp,Error,TEXT("Cannot find ambush location"));
		return;
	}
	// 텔레포트 + 플레이어를 바라보게
	Avatar->SetActorHiddenInGame(true);
	Avatar->SetActorLocation(Dest, false, nullptr, ETeleportType::TeleportPhysics);

	FRotator Look = (AmbushTarget->GetActorLocation() - Dest).Rotation();
	Look.Pitch = 0.f; Look.Roll = 0.f;
	Avatar->SetActorRotation(Look);

	if (UAnimInstance* Anim = Avatar->GetMesh()->GetAnimInstance())
	{
		Anim->Montage_Stop(0.1f);
	}
	Avatar->SetActorHiddenInGame(false);

	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		FGameplayCueParameters P;
		P.Location = Avatar->GetActorLocation();
		P.RawMagnitude = AppearDuration;
		ASC->ExecuteGameplayCue(CueAppear, P);
	}
	
	UAbilityTask_WaitDelay* Task = UAbilityTask_WaitDelay::WaitDelay(this, AppearDuration);
	Task->OnFinish.AddDynamic(this, &ThisClass::OnAppeared);
	Task->ReadyForActivation();
}

void UAIGA_BlinkAttack::OnAppeared()
{
	SetVisualsEnabled(true);

	if (AActor* Avatar = GetAvatarActorFromActorInfo())
	{
		FGameplayEventData Payload;
		Payload.Instigator = Avatar;
		Payload.Target     = AmbushTarget;

		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Avatar, AttackEventTag, Payload);
		return;
	}
	
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

bool UAIGA_BlinkAttack::FindAmbushLocation(AActor* Target, FVector& OutLocation) const
{
	ACharacter* Avatar = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Target || !Avatar) return false;

	UNavigationSystemV1* Nav = UNavigationSystemV1::GetCurrent(GetWorld());
	if (!Nav) return false;

	const FVector TargetLoc = Target->GetActorLocation();
	const FVector Fwd = Target->GetActorForwardVector();

	const UCapsuleComponent* Capsule = Avatar->GetCapsuleComponent();
	const float R = Capsule->GetScaledCapsuleRadius();
	const float H = Capsule->GetScaledCapsuleHalfHeight();

	// 정면 우선, 막히면 좌우로 벌려가며 시도
	static const float Angles[] = { 0.f, 25.f, -25.f, 50.f, -50.f };

	for (float Angle : Angles)
	{
		const FVector Dir = Fwd.RotateAngleAxis(Angle, FVector::UpVector);
		const FVector Desired = TargetLoc + Dir * AppearDistance;

		FNavLocation NavLoc;
		if (!Nav->ProjectPointToNavigation(Desired, NavLoc, FVector(200.f, 200.f, 400.f))) continue;

		const FVector Spawn = NavLoc.Location + FVector(0, 0, H);

		// 캡슐이 들어갈 자리인지 확인 — 벽이나 다른 적 속에 박히는 것 방지
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(Avatar);

		const bool bBlocked = GetWorld()->OverlapBlockingTestByChannel(
			Spawn, FQuat::Identity, ECC_Pawn,
			FCollisionShape::MakeCapsule(R, H), Params);

		if (bBlocked) continue;

		OutLocation = Spawn;
		return true;
	}
	return false;
}

void UAIGA_BlinkAttack::SetVisualsEnabled(bool bEnabled)
{
	if (!bVisualsDisabled && bEnabled) return;

	if (ACharacter* Avatar = Cast<ACharacter>(GetAvatarActorFromActorInfo()))
	{
		Avatar->SetActorHiddenInGame(!bEnabled);
		Avatar->SetActorEnableCollision(bEnabled);

		if (UCharacterMovementComponent* CMC = Avatar->GetCharacterMovement())
		{
			CMC->SetMovementMode(bEnabled ? MOVE_Walking : MOVE_None);
		}

		// 머티리얼도 즉시 복구 (Cue가 못 도는 취소 경로 대비)
		if (bEnabled)
		{
			if (AKernelEnemyCharacter* Enemy = Cast<AKernelEnemyCharacter>(Avatar))
			{
				Enemy->ResetDissolve();   // DissolveAmount를 0으로 즉시 설정
			}
		}
	}
	bVisualsDisabled = !bEnabled;
}