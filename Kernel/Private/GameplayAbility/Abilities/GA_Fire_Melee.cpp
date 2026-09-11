#include "GameplayAbility/Abilities/GA_Fire_Melee.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "GameplayAbility/KernelGameplayTags.h"
#include "Item/KernelEquipmentInstance.h"
#include "Item/KernelItemInstance.h"
#include "Item/KernelItemFragment_Melee.h"
#include "KernelCharacter/Hero/KernelHeroCharacter.h"

void UGA_Fire_Melee::Fire()
{
	Super::Fire();
	
	ComboIndex = 0;
	
	UE_LOG(LogTemp,Log,TEXT("Melee : fire"))
	if (!FireMontage1P)
	{
		UE_LOG(LogTemp, Error, TEXT("[Melee] FireMontage1P 없음 — Item=%s"),
			*GetNameSafe(EquipInst ? EquipInst->InstigatorItem : nullptr));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}
	
	if (!FireMontage3P)
	{
		UE_LOG(LogTemp, Error, TEXT("[Melee] FireMontage3P 없음 — Item=%s"),
			*GetNameSafe(EquipInst ? EquipInst->InstigatorItem : nullptr));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}
	
	UAbilityTask_PlayMontageAndWait* MontageTask =
	   UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, "Combo0", FireMontage3P);
	
	UAbilityTask_WaitGameplayEvent* WaitHitTag =
		UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, TAG_GameplayEvent_Melee_Hit);

	UAbilityTask_WaitGameplayEvent* WaitCombo =
	UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this, TAG_GameplayEvent_Melee_ComboWindow, nullptr, false);
	
	if (!MontageTask || !WaitHitTag)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}
	
	MontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnMontageCompleted);
	MontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageCancelled);
	MontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnMontageCancelled);
	WaitHitTag->EventReceived.AddDynamic(this, &ThisClass::OnMeleeHit);
	WaitHitTag->ReadyForActivation();
	
	WaitCombo->EventReceived.AddDynamic(this, &ThisClass::OnComboWindow);
	WaitCombo->ReadyForActivation();
	
	// 1P는 로컬 연출이므로 수동 재생
	if (CurrentActorInfo->IsLocallyControlled() && FireMontage1P)
	{
		if (IKernelCosmeticInterface* CosmeticTarget = Cast<IKernelCosmeticInterface>(GetAvatarActorFromActorInfo()))
		{
			if (USkeletalMeshComponent* Mesh1P = CosmeticTarget->GetMesh1P())
			{
				if (UAnimInstance* Anim1P = Mesh1P->GetAnimInstance())
				{
					Anim1P->Montage_Play(FireMontage1P);
				}
			}
		}
	}

	MontageTask->ReadyForActivation();   // [변경] 마지막에
}

void UGA_Fire_Melee::StartFireLoop()
{
	if (!CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}
	Fire();
}

void UGA_Fire_Melee::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
}

bool UGA_Fire_Melee::IsInputHeld() const
{
	const FGameplayAbilitySpec* Spec = GetCurrentAbilitySpec();
	return Spec && Spec->InputPressed;
}

void UGA_Fire_Melee::OnComboWindow(FGameplayEventData Payload)
{
	if (!IsInputHeld())
	{
		return;
	}

	// 매 타마다 코스트를 지불
	if (!CommitAbilityCost(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo))
	{
		return;   // 스태미나 부족 등 → 현재 섹션으로 마무리
	}

	ComboIndex = ComboSections.IsValidIndex(ComboIndex + 1) ? ComboIndex + 1 : 0;
	const FName Next = ComboSections[ComboIndex];

	MontageJumpToSection(Next);   // 3P — ASC를 거쳐 복제됨

	// 1P는 수동 재생이므로 따로 점프
	if (CurrentActorInfo->IsLocallyControlled() && FireMontage1P)
	{
		if (IKernelCosmeticInterface* Cosmetic = Cast<IKernelCosmeticInterface>(GetAvatarActorFromActorInfo()))
		{
			if (USkeletalMeshComponent* Mesh1P = Cosmetic->GetMesh1P())
			{
				if (UAnimInstance* Anim1P = Mesh1P->GetAnimInstance())
				{
					Anim1P->Montage_JumpToSection(Next, FireMontage1P);
				}
			}
		}
	}
}

void UGA_Fire_Melee::OnMontageCompleted()
{
	UE_LOG(LogTemp,Warning,TEXT("MontageEnd"))
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
	
	DrawDebugSphere(GetWorld(), EndLoc, TraceRadius, 32, bHit ? FColor::Red : FColor::Green, false, 1.f);
	
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
							TAG_Gameplay_Damage, Damage);
							
						(void) ApplyGameplayEffectSpecToTarget(
							CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, SpecHandle, TargetData);
					}
				}
			}
		}
	}
}