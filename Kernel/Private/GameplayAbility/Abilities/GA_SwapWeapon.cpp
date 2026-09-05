// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbility/Abilities/GA_SwapWeapon.h"
#include "KernelCharacter/Hero/KernelQuickBarComponent.h"

void UGA_SwapWeapon::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                     const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                     const FGameplayEventData* TriggerEventData)
{
	UE_LOG(LogTemp,Warning,TEXT("GA_SwapWeapon :: Activate"))
	
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		UE_LOG(LogTemp,Error,TEXT("GA_SwapWeapon :: Commit Ability Failed"))
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	if (!TriggerEventData)
	{
		UE_LOG(LogTemp, Error, TEXT("GA_SwapWeapon :: TriggerEventData is Null!"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	UKernelQuickBarComponent* QuickBar = GetAvatarActorFromActorInfo()->FindComponentByClass<UKernelQuickBarComponent>();
	if (!QuickBar)
	{
		UE_LOG(LogTemp,Error,TEXT("GA_SwapWeapon :: Cant find QuickBarComponent"))
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	QuickBar->SetActiveSlotIndex(TriggerEventData->EventMagnitude);
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
