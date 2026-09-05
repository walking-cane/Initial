// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbility/KernelAbilitySystemComponent.h"

UKernelAbilitySystemComponent::UKernelAbilitySystemComponent()
{
}

void UKernelAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	if (InputTag.IsValid())
	{
		ABILITYLIST_SCOPE_LOCK();
		
		for (FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			if (AbilitySpec.Ability && (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag)))
			{
				AbilitySpec.InputPressed = true;
				
				if (AbilitySpec.IsActive())
				{
					AbilitySpecInputPressed(AbilitySpec);
				}
				else
				{
					TryActivateAbility(AbilitySpec.Handle);
				}
			}
		}
	}
}

void UKernelAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (InputTag.IsValid())
	{
		ABILITYLIST_SCOPE_LOCK();
		
		for (FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			if (AbilitySpec.Ability && (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag)))
			{
				AbilitySpec.InputPressed = false;
				
				if (AbilitySpec.IsActive())
				{
					AbilitySpecInputReleased(AbilitySpec);
				}
			}
		}
	}
}

void UKernelAbilitySystemComponent::ClearInputAbility()
{
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
	InputHeldSpecHandles.Reset();
	
	UE_LOG(LogTemp,Warning,TEXT("[KernelASC] ClearAbility"));
}
