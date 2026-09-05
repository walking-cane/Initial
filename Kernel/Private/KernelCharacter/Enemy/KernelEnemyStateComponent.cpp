
#include "KernelCharacter/Enemy/KernelEnemyStateComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameplayAbility/KernelGameplayTags.h"
#include "Messages/KernelVerbMessage.h"

UKernelEnemyStateComponent::UKernelEnemyStateComponent()
{
	SetIsReplicatedByDefault(true);
}

void UKernelEnemyStateComponent::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
	DOREPLIFETIME(UKernelEnemyStateComponent, CurrentEnemyStateTag);
}

void UKernelEnemyStateComponent::SetEnemyStateTag(FGameplayTag NewStateTag)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		CurrentEnemyStateTag = NewStateTag;
		BroadcastStateChange();
	}
}

void UKernelEnemyStateComponent::BeginPlay()
{
	Super::BeginPlay();
	
	CurrentEnemyStateTag = FGameplayTag::RequestGameplayTag("Status.AI.Normal");
	
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
	if (ASC)
	{
		ASC->AddLooseGameplayTag(CurrentEnemyStateTag);
	}
}

void UKernelEnemyStateComponent::OnRep_EnemyStateTag(
	FGameplayTag OldEnemyStateTag)
{
	BroadcastStateChange();
}

void UKernelEnemyStateComponent::BroadcastStateChange()
{
	FKernelVerbMessage Message;
	Message.Verb = TAG_Kernel_EnemyState_Changed;
	Message.Instigator = GetOwner(); 
	Message.Tags.AddTag(CurrentEnemyStateTag);

	UGameplayMessageSubsystem::Get(GetWorld()).BroadcastMessage(Message.Verb, Message);
}