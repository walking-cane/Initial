// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbility/Attributes/KernelMovementSet.h"

#include "Net/UnrealNetwork.h"

UKernelMovementSet::UKernelMovementSet() : MoveSpeedMultiplier(1.0f)
{
	
}

void UKernelMovementSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION_NOTIFY(UKernelMovementSet, MoveSpeedMultiplier, COND_OwnerOnly, REPNOTIFY_Always);
}

void UKernelMovementSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	
	if (Attribute == GetMoveSpeedMultiplierAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.1f, 3.0f);
	}
}

void UKernelMovementSet::OnRep_MoveSpeedMultiplier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UKernelMovementSet, MoveSpeedMultiplier, OldValue);
}
