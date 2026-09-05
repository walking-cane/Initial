// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbility/Abilities/GA_Parry.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Cosmetics/KernelCosmeticInterface.h"

UGA_Parry::UGA_Parry()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void UGA_Parry::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle, 
	const FGameplayAbilityActorInfo* ActorInfo, 
	const FGameplayAbilityActivationInfo ActivationInfo, 
	const FGameplayEventData* TriggerEventData)
{
	UE_LOG(LogTemp,Warning,TEXT("GA_Parry :: Activate"))
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	if (ParryEffectClass)
	{
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(ParryEffectClass.LoadSynchronous());
		if (SpecHandle.IsValid())
		{
			(void)ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
		}
	}
	
	if (IKernelCosmeticInterface* CosmeticTarget = Cast<IKernelCosmeticInterface>(GetAvatarActorFromActorInfo()))
	{
		if (USkeletalMeshComponent* Mesh1P = CosmeticTarget->GetMesh1P())
		{
			if (UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance())
			{
				AnimInstance->Montage_Play(ParryMontage1P.LoadSynchronous());
			}
		}
	}
	
	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, NAME_None, ParryMontage3P.LoadSynchronous());
	if (MontageTask)
	{
		MontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnMontageCompleted);
		MontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnMontageCancelled);
		MontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageCancelled);
		MontageTask->ReadyForActivation();
		
		return;
	}
	
	EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
}

void UGA_Parry::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_Parry::OnMontageCancelled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}