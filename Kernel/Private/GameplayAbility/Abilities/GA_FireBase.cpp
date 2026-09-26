#include "GameplayAbility/Abilities/GA_FireBase.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemLog.h"
#include "GameplayCueManager.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Affixes/KernelAffixCombatLibrary.h"
#include "Animation/KernelAnimInstance.h"
#include "Item/KernelEquipmentInstance.h"
#include "Item/KernelItemInstance.h"
#include "Item/KernelItemFragment_Combat.h"
#include "Item/KernelItemFragment_Recoil.h"
#include "Item/KernelItemFragment_Weapon.h"
#include "KernelCharacter/Hero/KernelHeroWeaponComponent.h"
#include "Cosmetics/KernelCosmeticInterface.h"
#include "GameplayAbility/KernelGameplayTags.h"
#include "Item/KernelItemFragment_Cosmetic.h"

UGA_FireBase::UGA_FireBase()
{
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag("Status.Weapon.Firing"));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag("Cooldown.Weapon.Swap"));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag("Status.Weapon.Equipping"));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag("InputTag.Sprint"));
}

void UGA_FireBase::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	UE_LOG(LogAbilitySystem, Warning, TEXT("[FireBase] Activate"));
	
	InitializeWeapon();
	
	if (!EquipInst)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}
	
	FGameplayAbilitySpecHandle SpecHandle = CurrentSpecHandle;
	FPredictionKey ActivationPredictionKey = CurrentActivationInfo.GetActivationPredictionKey();
	
	// 서버: 클라이언트가 보낸 TargetData가 도착하면 OnTargetDataReady가 불리도록 대기
	if (CurrentActorInfo->IsNetAuthority() && !CurrentActorInfo->IsLocallyControlled())
	{
		CachedASC->AbilityTargetDataSetDelegate(SpecHandle, ActivationPredictionKey)
			.AddUObject(this, &ThisClass::OnTargetDataReplicated);
	}

	float CurrentTime = GetWorld()->GetTimeSeconds();
	float TimeSinceLastFire = CurrentTime - EquipInst->LastFireTime;
	
	// 만약 연사 속도보다 빨리 누르려 했다면 딜레이 후 발사하도록 Task 실행
	if (TimeSinceLastFire < FireDelay)
	{
		float RemainingDelay = FireDelay - TimeSinceLastFire;
		
		if (UAbilityTask_WaitDelay* WaitDelayTask = UAbilityTask_WaitDelay::WaitDelay(this, RemainingDelay))
		{
			WaitDelayTask->OnFinish.AddDynamic(this, &UGA_FireBase::StartFireLoop);
			WaitDelayTask->ReadyForActivation();
		}
		return;
	}
	
	if (CachedWeaponComp)
	{
		CachedWeaponComp->StartRecoil();
	}
	
	StartFireLoop();
}

void UGA_FireBase::StartFireLoop()
{
	if (!CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}
	
	PlayMuzzleCue();
	Fire();
	
	// FireDelay가 0이면 무한 루프가 되므로 하한을 둔다
	const float SafeDelay = FMath::Max(FireDelay, 0.05f);
	GetWorld()->GetTimerManager().SetTimer(FireTimerHandle, this, &UGA_FireBase::StartFireLoop, SafeDelay);
}

void UGA_FireBase::Fire()
{
	if (!EquipInst) return;
	
	EquipInst->LastFireTime = GetWorld()->GetTimeSeconds();
}

void UGA_FireBase::AddRecoilKick()
{
	if (IKernelCosmeticInterface* CosmeticTarget = Cast<IKernelCosmeticInterface>(GetAvatarActorFromActorInfo()))
	{
		if (USkeletalMeshComponent* Mesh1P = CosmeticTarget->GetMesh1P())
		{
			if (UKernelAnimInstance* AnimInstance = Cast<UKernelAnimInstance>(Mesh1P->GetAnimInstance()))
			{
				AnimInstance->AddRecoilKick(CachedRecoilKick, CachedMaxRecoilKick);
			}
		}
	}
}

void UGA_FireBase::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
							  const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);
	
	if (CachedWeaponComp)
	{
		CachedWeaponComp->StopRecoil();
	}
	
	UE_LOG(LogTemp,Log,TEXT("EndAbility"))
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_FireBase::InitializeWeapon()
{
	OwnerActor = GetAvatarActorFromActorInfo();
	if (!OwnerActor)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}
	
	CachedWeaponComp = OwnerActor->FindComponentByClass<UKernelHeroWeaponComponent>();
	if (!CachedWeaponComp)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}
	
	CachedASC = GetAbilitySystemComponentFromActorInfo_Ensured();
	if (!CachedASC)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}
	 
	EquipInst = Cast<UKernelEquipmentInstance>(GetCurrentSourceObject());
	if (!EquipInst || !EquipInst->InstigatorItem)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}
	
	const UKernelItemInstance* ItemInst = EquipInst->InstigatorItem;

	const UKernelItemFragment_Combat* StatFrag = ItemInst->FindFragmentByClass<UKernelItemFragment_Combat>();
	const UKernelItemFragment_Weapon* WeaponFrag = ItemInst->FindFragmentByClass<UKernelItemFragment_Weapon>();
	const UKernelItemFragment_Recoil* RecoilFrag = ItemInst->FindFragmentByClass<UKernelItemFragment_Recoil>();

	if (StatFrag)
	{
		FireDelay = StatFrag->FireDelay;
		Damage = StatFrag->BaseDamage + (ItemInst->Level * StatFrag->MultiplyStatsPerLevel);
	}
	
	if (WeaponFrag)
	{
		FireMontage1P = WeaponFrag->FireMontage1P;
		FireMontage3P = WeaponFrag->FireMontage3P;
	}

	if (RecoilFrag)
	{
		RecoilCurve = RecoilFrag->RecoilCurve;
		CachedRecoilKick = RecoilFrag->KickAmount;
		CachedMaxRecoilKick = RecoilFrag->MaxKickAmount;
	}
}

void UGA_FireBase::OnTargetDataReplicated(const FGameplayAbilityTargetDataHandle& Data, FGameplayTag ActivationTag)
{
	CachedASC->ConsumeClientReplicatedTargetData(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey());
	FGameplayAbilityTargetDataHandle LocalData = Data;
	OnTargetDataReady(LocalData);
}

void UGA_FireBase::OnTargetDataReady(const FGameplayAbilityTargetDataHandle& TargetData)
{
	if (!HasAuthority(&CurrentActivationInfo)) return;

	UAbilitySystemComponent* InstigatorASC = GetAbilitySystemComponentFromActorInfo();
	if (!InstigatorASC) return;
	
	UKernelItemInstance* SourceWeapon = EquipInst ? EquipInst->InstigatorItem : nullptr;

	FHitResult HitResult = UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(TargetData, 0);
	if (!HitResult.GetActor()) return;
	
	for (int32 i = 0; i < TargetData.Num(); ++i)
	{
		const FHitResult Hit =
			UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(TargetData, i);

		if (!Hit.bBlockingHit) continue;

		UAbilitySystemComponent* TargetASC =
			UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Hit.GetActor());
		if (!TargetASC) continue;

		if (TargetASC->HasMatchingGameplayTag(TAG_Status_Death_Dying)) continue;
		/*
		FGameplayEffectSpecHandle Spec = MakeOutgoingGameplayEffectSpec(DamageEffectClass);
		if (Spec.IsValid())
		{
			Spec.Data->SetSetByCallerMagnitude(TAG_Data_Damage, Damage);
			Spec.Data->GetContext().AddHitResult(Hit);
			InstigatorASC->ApplyGameplayEffectSpecToTarget(*Spec.Data.Get(), TargetASC);
		}
		*/
		
		FGameplayCueParameters FireParams;
		FireParams.Normal = Hit.ImpactNormal;
		FireParams.Location = Hit.ImpactPoint;
		FireParams.Instigator = OwnerActor;

		UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(
			OwnerActor, TAG_GameplayCue_Hit_Shield, EGameplayCueEvent::Executed, FireParams);
		
		UKernelAffixCombatLibrary::ApplyAffixOnHit(InstigatorASC, TargetASC, SourceWeapon, Hit);
	}
	
	FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass);
	if (SpecHandle.IsValid())
	{
		SpecHandle.Data->SetSetByCallerMagnitude(TAG_Gameplay_Damage, Damage);

		// GE에 붙은 큐들이 이 컨텍스트에서 위치/노멀/재질을 꺼내 쓴다.
		SpecHandle.Data->GetContext().AddHitResult(HitResult);

		(void)ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, SpecHandle, TargetData);
	}
}

bool UGA_FireBase::CalculateAimPoint(FVector& OutMuzzleLocation, FRotator& OutSpawnRotation) const
{
	if (!OwnerActor) return false;

	AController* C = OwnerActor->GetInstigatorController();
	if (!C) return false;   // AI·컨트롤러 없는 경우 방어

	FVector ViewLocation; 
	FRotator ViewRotation;
	C->GetPlayerViewPoint(ViewLocation, ViewRotation);
	const FVector AimEnd = ViewLocation + ViewRotation.Vector() * 10000.f;

	FHitResult AimHit;
	FCollisionQueryParams P;
	P.AddIgnoredActor(OwnerActor);
	const bool bHit = GetWorld()->LineTraceSingleByChannel(AimHit, ViewLocation, AimEnd, ECC_Camera, P);
	const FVector TargetPoint = bHit ? AimHit.ImpactPoint : AimEnd;

	USkeletalMeshComponent* MuzzleMesh = CurrentActorInfo->IsLocallyControlled() ? WeaponMesh1P : WeaponMesh3P;
	if (!MuzzleMesh) return false;

	OutMuzzleLocation = MuzzleMesh->GetSocketLocation(FName("Muzzle"));
	OutSpawnRotation = (TargetPoint - OutMuzzleLocation).Rotation();
	return true;
}

void UGA_FireBase::PlayMuzzleCue()
{
	if (!OwnerActor) return;

	FVector Loc;
	FRotator Rot;
	CalculateAimPoint(Loc, Rot);
	
	FGameplayCueParameters FireParams;
	FireParams.Normal = Rot.Vector();
	FireParams.Location = Loc;
	FireParams.Instigator = OwnerActor;

	UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(
		OwnerActor, TAG_GameplayCue_Flatline_Fire, EGameplayCueEvent::Executed, FireParams);
}