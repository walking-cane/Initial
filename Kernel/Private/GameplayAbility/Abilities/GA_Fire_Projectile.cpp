// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbility/Abilities/GA_Fire_Projectile.h"

#include "Item/KernelEquipmentInstance.h"
#include "Weapon/KernelProjectileBase.h"

void UGA_Fire_Projectile::Fire()
{
	Super::Fire();
	AddRecoilKick();
	
	if (!HasAuthority(&CurrentActivationInfo)) return;
	if (!ProjectileClass) return;
	
	FVector MuzzleLocation; 
	FRotator SpawnRotation;
	if (!CalculateAimPoint(MuzzleLocation, SpawnRotation)) return;
	
	FGameplayEffectSpecHandle Spec = MakeOutgoingGameplayEffectSpec(DamageEffectClass);
	if (Spec.IsValid())
	{
		Spec.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Data.Damage"), Damage);
	}
	
	const FTransform SpawnTM(SpawnRotation, MuzzleLocation);
	AKernelProjectileBase* Proj = GetWorld()->SpawnActorDeferred<AKernelProjectileBase>(
		ProjectileClass, SpawnTM, OwnerActor, Cast<APawn>(OwnerActor),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (!Proj) return;

	Proj->InitAsDamaging(GetAbilitySystemComponentFromActorInfo(), Spec, EquipInst ? EquipInst->InstigatorItem : nullptr, ProjectileSpeed);
	Proj->FinishSpawning(SpawnTM);
}
