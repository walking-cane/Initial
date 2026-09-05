// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbility/Abilities/AIGA_AttackBase.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "DrawDebugHelpers.h"
#include "GameplayAbility/KernelGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Engine/OverlapResult.h"

void UAIGA_AttackBase::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                     const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                     const FGameplayEventData* TriggerEventData)
{
	UE_LOG(LogTemp, Warning, TEXT("[GA_AttackBase] Activate"));
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (!GetAvatarActorFromActorInfo()) 
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}
	
	if (TriggerEventData && TriggerEventData->Target != nullptr)
	{
		UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this, NAME_None,AttackMontage);
		UAbilityTask_WaitGameplayEvent* WaitEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
				this,
				TAG_GameplayEvent_Melee_Hit,
				nullptr,
				true, 
				false);
				
		if (MontageTask && WaitEventTask)
		{
			MontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnMontageCompleted);
			MontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageCancelled);
			MontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnMontageCancelled);

			WaitEventTask->EventReceived.AddDynamic(this, &ThisClass::OnHitEventReceived);

			MontageTask->ReadyForActivation();
			WaitEventTask->ReadyForActivation();
					
			return;
		}
	}
	
	UE_LOG(LogTemp, Warning, TEXT("[GA_AttackBase] Task Failed"));
	EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
}

void UAIGA_AttackBase::OnMontageCompleted()
{
	UE_LOG(LogTemp, Warning, TEXT("[GA_AttackBase] Montage Completed"));
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UAIGA_AttackBase::OnMontageCancelled()
{
	UE_LOG(LogTemp, Warning, TEXT("[GA_AttackBase] Montage Cancelled"));
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UAIGA_AttackBase::OnHitEventReceived(FGameplayEventData Payload)
{
	if (!HasAuthorityOrPredictionKey(CurrentActorInfo, &CurrentActivationInfo)) return;
	AActor* Avatar = GetAvatarActorFromActorInfo();
	if (!Avatar) return;

	FVector StartLoc = Avatar->GetActorLocation();
	FVector Origin = StartLoc + Avatar->GetActorForwardVector() * AttackDistance;

	TArray<FOverlapResult> OverlapResults;
	FCollisionShape SphereShape = FCollisionShape::MakeSphere(AttackRadius);
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Avatar);

	bool bHit = GetWorld()->OverlapMultiByChannel(
		OverlapResults, 
		Origin, 
		FQuat::Identity, 
		ECC_Pawn,
		SphereShape, 
		Params
	);
	
	FColor DrawColor = bHit ? FColor::Green : FColor::Red;
	DrawDebugSphere(GetWorld(), Origin, AttackRadius, 32, DrawColor, false, 2.0f);

	if (bHit)
	{
		TSet<AActor*> HitActorsThisSwing;
		
		for (const FOverlapResult& Overlap : OverlapResults)
		{
			AActor* HitActor = Overlap.GetActor();
			
			if (HitActor && HitActor != Avatar && !HitActorsThisSwing.Contains(HitActor))
			{
				if (!HasAuthority(&CurrentActivationInfo)) return;
				
				HitActorsThisSwing.Add(HitActor);
		
				FGameplayAbilityTargetDataHandle TargetData = 
							UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(HitActor);

				FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass);
		
				if (SpecHandle.IsValid())
				{
					SpecHandle.Data.Get()->SetSetByCallerMagnitude(
						FGameplayTag::RequestGameplayTag(FName("Data.Damage")), Damage);

					(void) ApplyGameplayEffectSpecToTarget(
						CurrentSpecHandle,
						CurrentActorInfo, 
						CurrentActivationInfo, 
						SpecHandle,
						TargetData);
				}
			}
		}
	}
}
