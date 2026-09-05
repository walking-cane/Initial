// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbility/Abilities/Kallari/AIGA_Kallari_Backflip.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "GameFramework/Character.h"

void UAIGA_Kallari_Backflip::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                             const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                             const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	ACharacter* Avatar = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Avatar)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	Avatar->LandedDelegate.AddDynamic(this, &ThisClass::OnLanded);
	
	const FVector Backward = -Avatar->GetActorForwardVector();
	const FVector Launch   = Backward * BackSpeed + FVector::UpVector * UpSpeed;

	Avatar->LaunchCharacter(Launch, true, true);
	
	UAbilityTask_PlayMontageAndWait* Task = 
		UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, BackflipMontage);

	Task->OnCompleted.AddDynamic(this, &ThisClass::OnMontageEnded);
	Task->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageEnded);
	Task->OnCancelled.AddDynamic(this, &ThisClass::OnMontageEnded);
	Task->ReadyForActivation();
}

void UAIGA_Kallari_Backflip::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	if (ACharacter* Avatar = Cast<ACharacter>(GetAvatarActorFromActorInfo()))
	{
		Avatar->LandedDelegate.RemoveDynamic(this, &ThisClass::OnLanded);
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UAIGA_Kallari_Backflip::OnMontageEnded()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UAIGA_Kallari_Backflip::OnLanded(const FHitResult& Hit)
{
	if (ACharacter* Avatar = Cast<ACharacter>(GetAvatarActorFromActorInfo()))
	{
		Avatar->LandedDelegate.RemoveDynamic(this, &ThisClass::OnLanded);
	}
	
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
