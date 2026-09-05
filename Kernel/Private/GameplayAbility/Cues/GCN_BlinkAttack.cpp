// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbility/Cues/GCN_BlinkAttack.h"

#include "KernelCharacter/Enemy/KernelEnemyCharacter.h"

bool UGCN_BlinkAttack::OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Params) const
{
	if (AKernelEnemyCharacter* Character = Cast<AKernelEnemyCharacter>(MyTarget))
	{
		Character->StartDissolve(Params.RawMagnitude, bDissolveOut);
		return true;
	}
	return false;
}
