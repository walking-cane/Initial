// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbility/Abilities/GA_Crouch.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayAbility/KernelGameplayTags.h"
#include "Movement/KernelCharacterMovementComponent.h"

void UGA_Crouch::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                 const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	ACharacter* Char = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!ActorInfo || !Char)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	const bool bSprinting = GetAbilitySystemComponentFromActorInfo()->HasMatchingGameplayTag(TAG_Status_Sprint);
	
	Char->Crouch();
	
	if (!bSprinting)
	{
		return;   // 일반 크라우치.
	}
	
	// 슬라이드 진입
	if (UKernelCharacterMovementComponent* KernelCMC = Cast<UKernelCharacterMovementComponent>(Char->GetCharacterMovement()))
	{
		SlideStateHandle = KernelCMC->OnSlideStateChanged.AddUObject(this, &ThisClass::HandleSlideStateChanged);
		KernelCMC->SetCustomMovementMode(EKernelCustomMovementMode::Slide);
	}
}

void UGA_Crouch::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
	
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_Crouch::HandleSlideStateChanged(bool bIsSliding)
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC) { return; }

	if (bIsSliding)
	{
		ASC->AddGameplayCue(TAG_GameplayCue_Movement_Slide);
		UE_LOG(LogTemp,Log,TEXT("[Cue] CrouchGC Start"))
	}
	else
	{
		ASC->RemoveGameplayCue(TAG_GameplayCue_Movement_Slide);
		UE_LOG(LogTemp,Log,TEXT("[Cue] CrouchGC Remove"))
	}
}

void UGA_Crouch::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (ACharacter* Char = Cast<ACharacter>(GetAvatarActorFromActorInfo()))
	{
		if (UKernelCharacterMovementComponent* KernelCMC = Cast<UKernelCharacterMovementComponent>(Char->GetCharacterMovement()))
		{
			if (KernelCMC->IsCustomMovementMode(EKernelCustomMovementMode::Slide))
			{
				KernelCMC->SetMovementMode(MOVE_Walking);
			}
			
			KernelCMC->OnSlideStateChanged.Remove(SlideStateHandle);   // 바인딩 해제 필수
			SlideStateHandle.Reset();
		}
		
		Char->UnCrouch();
	}
	
	// 큐가 남아 있을 수 있으니 안전하게 제거
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		ASC->RemoveGameplayCue(TAG_GameplayCue_Movement_Slide);
	}

	if (SpeedHandle.IsValid() && ActorInfo && ActorInfo->AbilitySystemComponent.IsValid())
	{
		ActorInfo->AbilitySystemComponent->RemoveActiveGameplayEffect(SpeedHandle);
		SpeedHandle.Invalidate();
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Crouch::OnMontageCancelled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}