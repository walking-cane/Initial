  // Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbility/KernelDamageExecution.h"
#include "GameplayAbility/Attributes/KernelHealthAttributeSet.h"
#include "AbilitySystemComponent.h"

  void UKernelDamageExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
                                                      FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	Super::Execute_Implementation(ExecutionParams, OutExecutionOutput);
  	
	UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
	UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
  	
  	if (!TargetASC || !SourceASC) return;
  //	UE_LOG(LogTemp,Warning,TEXT("ExecCalc :: Damage Received"))

	FGameplayTagContainer TargetTags;
	TargetASC->GetOwnedGameplayTags(TargetTags);
  	
  	float BaseDamage = ExecutionParams.GetOwningSpec().GetSetByCallerMagnitude(
  		FGameplayTag::RequestGameplayTag(FName("Data.Damage")), 
		false, 
		0.f);
  	
  	if (TargetTags.HasTag(FGameplayTag::RequestGameplayTag("State.Parrying")))
  	{
  		UE_LOG(LogTemp,Warning,TEXT("ExecCalc :: Parry Succeeded!"))
  		
  		FGameplayEventData ParryPayload;
  		ParryPayload.Instigator = SourceASC->GetAvatarActor();
  		TargetASC->HandleGameplayEvent(
			  FGameplayTag::RequestGameplayTag("Event.Defense.ParrySuccess"), &ParryPayload);

  		FGameplayEventData StunPayload;
  		StunPayload.Target = TargetASC->GetAvatarActor();
  		SourceASC->HandleGameplayEvent(
			  FGameplayTag::RequestGameplayTag("Event.Attack.Parried"), &StunPayload);

  		return;
  	}
  	
  	// Damage Calculate logic.
	if (BaseDamage > 0.f)
	{
		BaseDamage = FMath::RoundToInt(FMath::RandRange(BaseDamage, BaseDamage + 5.f));
		
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(
			UKernelHealthAttributeSet::GetDamageAttribute(), EGameplayModOp::Additive, BaseDamage));
		
		UE_LOG(LogTemp,Log,TEXT("[DmgExec] Damage Executed"))
	}
}
