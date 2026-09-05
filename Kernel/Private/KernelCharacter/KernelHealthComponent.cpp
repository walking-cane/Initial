// Fill out your copyright notice in the Description page of Project Settings.


#include "KernelCharacter/KernelHealthComponent.h"
#include "GameplayAbility/KernelAbilitySystemComponent.h"
#include "GameplayAbility/Attributes/KernelHealthAttributeSet.h"

UKernelHealthComponent::UKernelHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UKernelHealthComponent::InitializeWithAbilitySystem(UKernelAbilitySystemComponent* InASC)
{
	if (CachedASC == InASC && HealthSet) return;
	
	CachedASC = InASC;
	
	if (CachedASC)
	{
		HealthSet = CachedASC->GetSet<UKernelHealthAttributeSet>();
		if (HealthSet)
		{
			UKernelHealthAttributeSet* MutableHealthSet = const_cast<UKernelHealthAttributeSet*>(HealthSet);
			MutableHealthSet->SetMaxHealth(HealthAmount);
			MutableHealthSet->SetHealth(HealthAmount);
			MutableHealthSet->OnHealthChanged.AddUObject(this, &UKernelHealthComponent::HandleHealthChanged);
			MutableHealthSet->OnOutOfHealth.AddUObject(this, &UKernelHealthComponent::HandleOutOfHealth);
		}
	}
}

void UKernelHealthComponent::HandleOutOfHealth(AActor* DamageInstigator, 
		AActor* DamageCauser,
		const FGameplayEffectSpec* DamageEffectSpec,
		float DamageMagnitude,
		float OldValue,
		float NewValue)
{
	if (CachedASC && DamageEffectSpec)
	{
		FGameplayEventData Payload;
		Payload.EventTag = FGameplayTag::RequestGameplayTag("Event.Death");
		Payload.Instigator = DamageInstigator;
		Payload.EventMagnitude = DamageMagnitude;
		
		CachedASC->HandleGameplayEvent(Payload.EventTag, &Payload);
		UE_LOG(LogTemp,Warning,TEXT("HealthComp :: Send Cue Succeed"));
	}
	
	OnDeathStarted.Broadcast(GetOwner());
}

void UKernelHealthComponent::HandleHealthChanged(
	AActor* Instigator, AActor* Causer, 
	const FGameplayEffectSpec* EffectSpec, 
	float Magnitude, float OldValue, float NewValue)
{
	//델리게이트 간소화 후 전송
	OnHealthChanged.Broadcast(this, OldValue, NewValue, Instigator);
}

float UKernelHealthComponent::GetHealthPercent() const
{
	if (HealthSet && HealthSet->GetMaxHealth() > 0.0f)
	{
		return HealthSet->GetHealth() / HealthSet->GetMaxHealth();
	}
	return 0.0f;
}