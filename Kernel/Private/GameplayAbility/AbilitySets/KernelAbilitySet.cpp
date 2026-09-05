// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbility/AbilitySets/KernelAbilitySet.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemLog.h"
#include "Abilities/GameplayAbility.h"

void UKernelAbilitySet::GiveToAbilitySystem(
	UAbilitySystemComponent* ASC,
	TArray<FGameplayAbilitySpecHandle>* OutGrantedHandles,
	UObject* SourceObject) const
{
	check(ASC);

	for (const FHeroAbilitySet& AbilityToGrant : GrantedAbilities)
	{
		if (!AbilityToGrant.Ability) continue;
		
		FGameplayAbilitySpec AbilitySpec(
			AbilityToGrant.Ability, AbilityToGrant.AbilityLevel, INDEX_NONE, SourceObject);
		
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(AbilityToGrant.InputTag);

		FGameplayAbilitySpecHandle Handle = ASC->GiveAbility(AbilitySpec);

		if (OutGrantedHandles)
		{
			OutGrantedHandles->Add(Handle);
		}
		
		UE_LOG(LogAbilitySystem, Log, TEXT("[AbilitySet_Hero] GiveAbility : %s"), *AbilityToGrant.InputTag.ToString())
	}
}
