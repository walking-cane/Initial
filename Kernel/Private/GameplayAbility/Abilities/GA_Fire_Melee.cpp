#include "GameplayAbility/Abilities/GA_Fire_Melee.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Item/KernelEquipmentInstance.h"
#include "Item/KernelItemInstance.h"
#include "Item/KernelItemFragment_Melee.h"
#include "KernelCharacter/Hero/KernelHeroCharacter.h"

void UGA_Fire_Melee::Fire()
{
	Super::Fire();
	
	UAbilityTask_PlayMontageAndWait* MontageTask = 
		UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, FireMontage);
		
	UAbilityTask_WaitGameplayEvent* WaitTag = 
		UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, FGameplayTag::RequestGameplayTag("Event.Melee.Hit"));
	
	if (AKernelHeroCharacter* Hero = Cast<AKernelHeroCharacter>(GetAvatarActorFromActorInfo()))
	{
		if (Hero->IsLocallyControlled())
		{
			if (UAnimInstance* AnimInstance1P = Hero->GetMesh1P()->GetAnimInstance())
			{
				AnimInstance1P->Montage_Play(FireMontage);
			}
		}
	}
	
	if (MontageTask && WaitTag)
	{
		MontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnMontageCompleted);
		MontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageCancelled);
		MontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnMontageCancelled);
		
		WaitTag->EventReceived.AddDynamic(this, &ThisClass::OnMeleeHit);
		
		MontageTask->ReadyForActivation();
		WaitTag->ReadyForActivation();
	}
}

void UGA_Fire_Melee::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_Fire_Melee::OnMontageCancelled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UGA_Fire_Melee::OnMeleeHit(FGameplayEventData Payload)
{
	if (!EquipInst || !EquipInst->InstigatorItem) return;

	float TraceRadius = 50.f;
	float TraceDistance = 150.f;
	
	const UKernelItemFragment_Melee* MeleeFrag = 
		EquipInst->InstigatorItem->FindFragmentByClass<UKernelItemFragment_Melee>();
		
	if (MeleeFrag)
	{
		TraceRadius = MeleeFrag->MeleeRadius;
		TraceDistance = MeleeFrag->MeleeDistance;
	}
	
	TArray<FHitResult> HitResults;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(OwnerActor);
	
	FVector StartLoc = OwnerActor->GetActorLocation() + FVector(0.f, 0.f, 40.f);
	FVector EndLoc = StartLoc + OwnerActor->GetActorForwardVector() * TraceDistance;
	
	bool bHit = GetWorld()->SweepMultiByChannel(
		HitResults, StartLoc, EndLoc, FQuat::Identity, ECC_Pawn,
		FCollisionShape::MakeSphere(TraceRadius), Params);
	
	if (bHit && HitResults.Num() > 0)
	{
		TSet<AActor*> HitActorsThisSwing;

		for (const FHitResult& FinalResult : HitResults)
		{
			AActor* HitActor = FinalResult.GetActor();

			if (HitActor && HitActor != OwnerActor && !HitActorsThisSwing.Contains(HitActor))
			{
				HitActorsThisSwing.Add(HitActor);
				
				// 서버 권한일 때만 데미지를 적용
				if (HasAuthority(&CurrentActivationInfo))
				{
					FGameplayAbilityTargetDataHandle TargetData = 
						UAbilitySystemBlueprintLibrary::AbilityTargetDataFromHitResult(FinalResult);
				
					FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass);
				
					if (SpecHandle.IsValid())
					{
						// GA_FireBase에서 이미 계산된 Damage 변수 사용
						SpecHandle.Data.Get()->SetSetByCallerMagnitude(
							FGameplayTag::RequestGameplayTag("Data.Damage"), Damage);
							
						(void) ApplyGameplayEffectSpecToTarget(
							CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, SpecHandle, TargetData);
					}
				}
			}
		}
	}
}