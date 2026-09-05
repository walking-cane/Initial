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
	UE_LOG(LogTemp, Error, TEXT("GA_EquipWeapon :: Activate"))
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	Player = TriggerEventData ? TriggerEventData->Instigator : nullptr;
	if (!TriggerEventData || !Player)
	{
		UE_LOG(LogTemp, Error, TEXT("GA_EquipWeapon :: No Data"))
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	UKernelEquipmentInstance* EquipInstance = Cast<UKernelEquipmentInstance>(const_cast<UObject*>(TriggerEventData->OptionalObject.Get()));
	if (!EquipInstance || !EquipInstance->InstigatorItem)
	{
		UE_LOG(LogTemp, Error, TEXT("GA_EquipWeapon :: Invalid Equipment Instance"))
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	CosmeticFrag = EquipInstance->InstigatorItem->FindFragmentByClass<UKernelItemFragment_Cosmetic>();
	if (!CosmeticFrag)
	{
		UE_LOG(LogTemp, Error, TEXT("GA_EquipWeapon :: No Equippable Fragment found!"))
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	WeaponFrag = EquipInstance->InstigatorItem->FindFragmentByClass<UKernelItemFragment_Weapon>();
	if (!WeaponFrag)
	{
		UE_LOG(LogTemp, Error, TEXT("GA_EquipWeapon :: No Weapon Fragment found!"))
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	UKernelCosmeticComponent* CosmeticComp = Player->FindComponentByClass<UKernelCosmeticComponent>();
	if (!CosmeticComp)
	{
		UE_LOG(LogTemp, Error, TEXT("GA_EquipWeapon :: Cant find CosmeticComponent!"))
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	if (CosmeticFrag->SpawnMesh.IsValid())
	{
		CosmeticComp->ChangeWeapon(CosmeticFrag->SpawnMesh.LoadSynchronous());
		UE_LOG(LogTemp,Warning,TEXT("[GA_EquipWeapon] ChangeWeapon"))
	}
	
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