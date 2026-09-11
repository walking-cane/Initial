// Fill out your copyright notice in the Description page of Project Settings.

#include "GameplayAbility/Abilities/GA_EquipWeapon.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Item/KernelEquipmentInstance.h"
#include "Item/KernelItemFragment_Cosmetic.h"
#include "Item/KernelItemInstance.h"
#include "Item/KernelItemFragment_Weapon.h"
#include "Cosmetics/KernelCosmeticComponent.h"
#include "KernelCharacter/Hero/KernelHeroCharacter.h"

UGA_EquipWeapon::UGA_EquipWeapon()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
}

void UGA_EquipWeapon::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                      const FGameplayAbilityActorInfo* ActorInfo,
                                      const FGameplayAbilityActivationInfo ActivationInfo,
                                      const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	Player = TriggerEventData ? TriggerEventData->Instigator : nullptr;
	if (!TriggerEventData || !Player)
	{
		UE_LOG(LogTemp, Error, TEXT("GA_EquipWeapon :: No Data"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UKernelEquipmentInstance* EquipInstance =
		Cast<UKernelEquipmentInstance>(const_cast<UObject*>(TriggerEventData->OptionalObject.Get()));
	if (!EquipInstance || !EquipInstance->InstigatorItem)
	{
		UE_LOG(LogTemp, Error, TEXT("GA_EquipWeapon :: Invalid Equipment Instance"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	CosmeticFrag = EquipInstance->InstigatorItem->FindFragmentByClass<UKernelItemFragment_Cosmetic>();
	if (!CosmeticFrag)
	{
		UE_LOG(LogTemp, Error, TEXT("GA_EquipWeapon :: No Cosmetic Fragment found!"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	WeaponFrag = EquipInstance->InstigatorItem->FindFragmentByClass<UKernelItemFragment_Weapon>();
	if (!WeaponFrag)
	{
		UE_LOG(LogTemp, Error, TEXT("GA_EquipWeapon :: No Weapon Fragment found!"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UKernelCosmeticComponent* CosmeticComp = Player->FindComponentByClass<UKernelCosmeticComponent>();
	if (!CosmeticComp)
	{
		UE_LOG(LogTemp, Error, TEXT("GA_EquipWeapon :: Cant find CosmeticComponent!"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	TArray<FKernelWeaponAttachEntry> Entries;
	Entries.Reserve(CosmeticFrag->AttachRules.Num());

	// 건틀릿, 쌍검같은 무기를 여러개 부착하는 경우를 위해 반복문
	for (const FKernelWeaponAttachRule& Rule : CosmeticFrag->AttachRules)
	{
		if (Rule.WeaponActorClass.IsNull())
		{
			continue;
		}

		UClass* LoadedClass = Rule.WeaponActorClass.LoadSynchronous();
		if (!LoadedClass)
		{
			UE_LOG(LogTemp, Error, TEXT("GA_EquipWeapon :: Failed to load %s"),
				*Rule.WeaponActorClass.ToString());
			continue;
		}

		FKernelWeaponAttachEntry& Entry = Entries.AddDefaulted_GetRef();
		Entry.WeaponActorClass = LoadedClass;
		Entry.AttachSocket1P = Rule.AttachSocket1P;
		Entry.AttachSocket3P = Rule.AttachSocket3P;
		Entry.AttachTransform = Rule.AttachTransform;
	}

	// 빈 배열이어도 호출한다. 이전 무기를 정리하는 경로이기도 하다.
	CosmeticComp->SetWeaponAttachEntries(Entries);

	CosmeticComp->ApplyWeaponLayer(WeaponFrag->WeaponLayer1P, WeaponFrag->WeaponLayer3P);
	CosmeticComp->PlayEquipMontage(WeaponFrag->EquipMontage);

	if (EquipLockEffect && WeaponFrag->EquipMontage)
	{
		const float LockDuration = WeaponFrag->EquipMontage->GetPlayLength() - 0.3f;

		FGameplayEffectSpecHandle Spec = MakeOutgoingGameplayEffectSpec(EquipLockEffect);
		if (Spec.IsValid())
		{
			Spec.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Cooldown.Equip"), LockDuration);
			(void)ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, Spec);
		}
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}