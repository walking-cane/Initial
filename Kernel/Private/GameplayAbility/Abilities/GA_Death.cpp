// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbility/Abilities/GA_Death.h"
#include "AbilitySystemComponent.h"
#include"GameplayAbility/KernelGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "KernelCharacter/Enemy/KernelEnemyCharacter.h"
#include "Cosmetics/KernelCosmeticInterface.h"
#include "GameFramework/PawnMovementComponent.h"


void UGA_Death::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	UE_LOG(LogTemp,Warning,TEXT("GA_Death :: Activate"))
	
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (!TriggerEventData)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	if (IKernelCosmeticInterface* CosmeticTarget = Cast<IKernelCosmeticInterface>(GetAvatarActorFromActorInfo()))
	{
		if (UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this, NAME_None, CosmeticTarget->GetDeathMontage()))
		{
			GetAbilitySystemComponentFromActorInfo()->AddLooseGameplayTag(TAG_Status_Death_Dying);
			
			MontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnMontageEnded);
			MontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnMontageEnded);
			MontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageEnded);
			MontageTask->ReadyForActivation();
			
			UE_LOG(LogTemp,Warning,TEXT("GA_Death :: OnDeath Started"))
		}
		
		if (UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get())
		{
			FGameplayCueParameters CueParams;
			
			CueParams.RawMagnitude = TriggerEventData->EventMagnitude; //Damage
			CueParams.Location = GetAvatarActorFromActorInfo()->GetActorLocation();
			
			ASC->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.Event.Death"), CueParams);
		}

		if (AKernelEnemyCharacter* Enemy = Cast<AKernelEnemyCharacter>(GetAvatarActorFromActorInfo()))
		{
			Enemy->DropItemOnDeath();
			Enemy->HandleDeathStarted();
		}
		
		return;
	}
	
	EndAbility(Handle, ActorInfo, ActivationInfo, true,false);
}

void UGA_Death::OnMontageEnded()
{
	UE_LOG(LogTemp,Warning,TEXT("[GA_Death] OnMontageEnded"))
	if (AActor* Avatar = GetAvatarActorFromActorInfo())
	{
		Avatar->SetActorHiddenInGame(true);
		
		if (APawn* AvatarPawn = Cast<APawn>(Avatar))
		{
			AvatarPawn->DetachFromControllerPendingDestroy();
		}
		
		Avatar->SetLifeSpan(0.1f);
	}
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}