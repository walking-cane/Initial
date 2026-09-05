// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbility/Attributes/KernelCombatAttributeSet.h"
#include "Net/UnrealNetwork.h"

UKernelCombatAttributeSet::UKernelCombatAttributeSet()
	: BaseDamage(0.0f)
	, BaseHeal(0.0f), MovementSpeed(300.f)
{
}

void UKernelCombatAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UKernelCombatAttributeSet, BaseDamage,COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UKernelCombatAttributeSet, BaseHeal,COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UKernelCombatAttributeSet, MovementSpeed, COND_None, REPNOTIFY_Always);
}

void UKernelCombatAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	
	if (Attribute == GetMovementSpeedAttribute())
	{
		NewValue = FMath::Max(NewValue, 0.f);
	}
}

void UKernelCombatAttributeSet::OnRep_BaseDamage(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UKernelCombatAttributeSet, BaseDamage, OldValue);
}

void UKernelCombatAttributeSet::OnRep_BaseHeal(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UKernelCombatAttributeSet, BaseHeal, OldValue);
}

void UKernelCombatAttributeSet::OnRep_MovementSpeed(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UKernelCombatAttributeSet, MovementSpeed, OldValue);
}