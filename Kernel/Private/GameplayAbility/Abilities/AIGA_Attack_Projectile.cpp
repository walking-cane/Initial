// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbility/Abilities/AIGA_Attack_Projectile.h"
#include "AbilitySystemLog.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "GameplayAbility/KernelGameplayTags.h"
#include "Weapon/KernelProjectileBase.h"

UAIGA_Attack_Projectile::UAIGA_Attack_Projectile()
{
	InstancingPolicy   = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
}

void UAIGA_Attack_Projectile::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                              const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                              const FGameplayEventData* TriggerEventData)
{
	UE_LOG(LogAbilitySystem, Warning, TEXT("[AI_Projectile] Activate"))
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (!GetAvatarActorFromActorInfo())
	{
		UE_LOG(LogAbilitySystem, Error, TEXT("[AIGA_AttackProjectile] AvatarActor is NULL!"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	CachedTarget = TriggerEventData->Target;
	
	if (!InitFireInfo())
	{
		UE_LOG(LogAbilitySystem, Error, TEXT("[AIGA_AttackProjectile] Cant init Fire info"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		UE_LOG(LogAbilitySystem, Error, TEXT("[AIGA_AttackProjectile] Commit failed!"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	if (!TriggerEventData || TriggerEventData->Target == nullptr)
	{
		UE_LOG(LogAbilitySystem, Error, TEXT("[AIGA_AttackProjectile] TriggerEventData is NULL!"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	if (!FireMontage)
	{
		UE_LOG(LogAbilitySystem, Error, TEXT("[AIGA_AttackProjectile] FireMontage is NULL!"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, NAME_None, FireMontage);
	UAbilityTask_WaitGameplayEvent* WaitTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this, TAG_AI_FireProjectile);
		
	MontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnMontageCancelled);
	MontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageCancelled);
	MontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnMontageCompleted);
	WaitTask->EventReceived.AddDynamic(this, &ThisClass::OnEventReceived);
		
	WaitTask->ReadyForActivation();
	MontageTask->ReadyForActivation();
}

bool UAIGA_Attack_Projectile::InitFireInfo()
{
	USkeletalMeshComponent* Mesh = CurrentActorInfo ? CurrentActorInfo->SkeletalMeshComponent.Get() : nullptr;
	if (Mesh == nullptr)
	{
		UE_LOG(LogAbilitySystem, Error, TEXT("[AIGA_AttackProjectile] Mesh is null"));
		return false;
	}
	
	if (CachedTarget == nullptr)
	{
		UE_LOG(LogAbilitySystem, Error, TEXT("[AIGA_AttackProjectile] CachedTarget is null"));
		return false;
	}
	
	const APawn* TargetPawn = Cast<APawn>(CachedTarget);
	if (TargetPawn == nullptr)
	{
		UE_LOG(LogAbilitySystem, Error, TEXT("[AIGA_AttackProjectile] TargetPawn is NULL"));
		return false;
	}
	
	Loc = Mesh->GetSocketLocation(FName("Muzzle"));
	Rot = (TargetPawn->GetPawnViewLocation() - Loc).Rotation();
	return true;
}

FRotator UAIGA_Attack_Projectile::ApplySpread(const FRotator& BaseRot) const
{
	if (SpreadHalfAngle <= 0.f) return BaseRot;

	const FVector Cone = FMath::VRandCone(BaseRot.Vector(), FMath::DegreesToRadians(SpreadHalfAngle));
	return Cone.Rotation();
}

void UAIGA_Attack_Projectile::OnMontageCancelled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UAIGA_Attack_Projectile::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UAIGA_Attack_Projectile::OnEventReceived(FGameplayEventData Payload)
{
	AActor* OwnerActor = GetAvatarActorFromActorInfo();
	
	FGameplayEffectSpecHandle Spec = MakeOutgoingGameplayEffectSpec(DamageEffectClass);
	
	if (Spec.IsValid())
	{
		Spec.Data->SetSetByCallerMagnitude(TAG_Gameplay_Damage, Damage);
	}
	
	const FRotator FinalRot = ApplySpread(Rot);
	
	const FTransform SpawnTM(FinalRot, Loc);
	AKernelProjectileBase* Proj = GetWorld()->SpawnActorDeferred<AKernelProjectileBase>(
		ProjectileClass, SpawnTM, OwnerActor, Cast<APawn>(OwnerActor),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	
	if (!Proj)
	{
		UE_LOG(LogTemp,Error,TEXT("[AI_Projectile] SpawnActor Failed"));
		return;
	}

	Proj->InitAsDamaging(GetAbilitySystemComponentFromActorInfo(), Spec, nullptr, ProjectileSpeed);
	Proj->FinishSpawning(SpawnTM);
	
	// For visual (Replicated)
	FGameplayCueParameters CueParams;
	CueParams.Location = Loc;
	CueParams.Normal = FinalRot.Vector();
	
	UGameplayAbility::K2_ExecuteGameplayCueWithParams(TAG_GameplayCue_Projectile, CueParams);
}
