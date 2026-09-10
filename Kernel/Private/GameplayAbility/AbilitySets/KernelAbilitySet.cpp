#include "GameplayAbility/AbilitySets/KernelAbilitySet.h"
#include "AbilitySystemLog.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayEffect.h"
#include "GameplayAbility/KernelAbilitySystemComponent.h"

void FKernelAbilitySet_GrantedHandles::AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle)
{
	if (Handle.IsValid())
	{
		AbilitySpecHandles.Add(Handle);
	}
}

void FKernelAbilitySet_GrantedHandles::AddGameplayEffectHandle(const FActiveGameplayEffectHandle& Handle)
{
	if (Handle.IsValid())
	{
		GameplayEffectHandles.Add(Handle);
	}
}

void FKernelAbilitySet_GrantedHandles::AddAttributeSet(UAttributeSet* Set)
{
	GrantedAttributeSets.Add(Set);
}

void FKernelAbilitySet_GrantedHandles::TakeFromAbilitySystem(UKernelAbilitySystemComponent* KernelASC)
{
	check(KernelASC);

	if (!KernelASC->IsOwnerActorAuthoritative())
	{
		return;
	}

	for (const FGameplayAbilitySpecHandle& Handle : AbilitySpecHandles)
	{
		if (Handle.IsValid())
		{
			KernelASC->ClearAbility(Handle);
		}
	}

	for (const FActiveGameplayEffectHandle& Handle : GameplayEffectHandles)
	{
		if (Handle.IsValid())
		{
			KernelASC->RemoveActiveGameplayEffect(Handle);
		}
	}

	for (UAttributeSet* Set : GrantedAttributeSets)
	{
		KernelASC->RemoveSpawnedAttribute(Set);
	}

	AbilitySpecHandles.Reset();
	GameplayEffectHandles.Reset();
	GrantedAttributeSets.Reset();
}

void UKernelAbilitySet::GiveToAbilitySystem(
	UKernelAbilitySystemComponent* KernelASC,
	FKernelAbilitySet_GrantedHandles* OutGrantedHandles,
	UObject* SourceObject) const
{
	check(KernelASC);

	if (!KernelASC->IsOwnerActorAuthoritative())
	{
		return;
	}

	// Grant the gameplay abilities.
	for (int32 AbilityIndex = 0; AbilityIndex < GrantedAbilities.Num(); ++AbilityIndex)
	{
		const FKernelAbilitySet_GameplayAbility& AbilityToGrant = GrantedAbilities[AbilityIndex];

		if (!IsValid(AbilityToGrant.Ability))
		{
			UE_LOG(LogAbilitySystem, Error,
				TEXT("GrantedAbilities[%d] on ability set [%s] is not valid."),
				AbilityIndex, *GetNameSafe(this));
			continue;
		}

		UGameplayAbility* AbilityCDO = AbilityToGrant.Ability->GetDefaultObject<UGameplayAbility>();

		FGameplayAbilitySpec AbilitySpec(AbilityCDO, AbilityToGrant.AbilityLevel);
		AbilitySpec.SourceObject = SourceObject;
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(AbilityToGrant.InputTag);

		const FGameplayAbilitySpecHandle AbilitySpecHandle = KernelASC->GiveAbility(AbilitySpec);

		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddAbilitySpecHandle(AbilitySpecHandle);
		}
	}

	// Grant the gameplay effects.
	for (int32 EffectIndex = 0; EffectIndex < GrantedEffects.Num(); ++EffectIndex)
	{
		const FKernelAbilitySet_GameplayEffect& EffectToGrant = GrantedEffects[EffectIndex];

		if (!IsValid(EffectToGrant.GameplayEffect))
		{
			UE_LOG(LogAbilitySystem, Error,
				TEXT("GrantedEffects[%d] on ability set [%s] is not valid"),
				EffectIndex, *GetNameSafe(this));
			continue;
		}

		const UGameplayEffect* GameplayEffect = EffectToGrant.GameplayEffect->GetDefaultObject<UGameplayEffect>();
		const FActiveGameplayEffectHandle GameplayEffectHandle = KernelASC->ApplyGameplayEffectToSelf(
			GameplayEffect, EffectToGrant.EffectLevel, KernelASC->MakeEffectContext());

		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddGameplayEffectHandle(GameplayEffectHandle);
		}
	}
}