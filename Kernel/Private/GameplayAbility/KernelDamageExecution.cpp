  // Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbility/KernelDamageExecution.h"
#include "GameplayAbility/Attributes/KernelHealthAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "GameplayAbility/KernelGameplayTags.h"

  void UKernelDamageExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
                                                      FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	Super::Execute_Implementation(ExecutionParams, OutExecutionOutput);
  	
	UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
	UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
  	
  	if (!TargetASC || !SourceASC) return;

	FGameplayTagContainer TargetTags;
	TargetASC->GetOwnedGameplayTags(TargetTags);
  	
  	float BaseDamage = ExecutionParams.GetOwningSpec().GetSetByCallerMagnitude(
  		TAG_Gameplay_Damage, 
		false, 
		0.f);
  	
  	// Damage Calculate logic.
	if (BaseDamage > 0.f)
	{
		BaseDamage = FMath::RoundToInt(FMath::RandRange(BaseDamage, BaseDamage + BaseDamage * 0.05f));
		
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(
			UKernelHealthAttributeSet::GetDamageAttribute(), EGameplayModOp::Additive, BaseDamage));
		
		UE_LOG(LogTemp,Log,TEXT("[DmgExec] Damage Executed"))
	}
}
