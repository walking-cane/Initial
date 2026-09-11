// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbility/Abilities/AIGA_Attack_Projectile.h"
#include "AbilitySystemLog.h"

void UAIGA_Attack_Projectile::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                              const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                              const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (!GetAvatarActorFromActorInfo())
	{
		UE_LOG(LogAbilitySystem, Error, TEXT("[AIGA_AttackProjectile] AvatarActor is NULL!"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
	
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		UE_LOG(LogAbilitySystem, Error, TEXT("[AIGA_AttackProjectile] Commit failed!"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
	
	if (FireMontage)
	{
		
	}
}

void UAIGA_Attack_Projectile::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

bool UAIGA_Attack_Projectile::InitFireInfo()
{
	
	return false;
}
