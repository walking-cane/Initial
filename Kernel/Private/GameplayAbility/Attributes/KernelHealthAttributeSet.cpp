// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbility/Attributes/KernelHealthAttributeSet.h"

#include "AbilitySystemLog.h"
#include "GameplayEffectExtension.h"
#include "NativeGameplayTags.h"
#include "GameplayAbility/KernelGameplayTags.h"
#include "Messages/KernelVerbMessage.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Net/UnrealNetwork.h"

UKernelHealthAttributeSet::UKernelHealthAttributeSet()
	: Health(100.0f)
	, MaxHealth(100.0f)
{
	bOutOfHealth = false;
}

void UKernelHealthAttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION_NOTIFY(UKernelHealthAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UKernelHealthAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
}

bool UKernelHealthAttributeSet::PreGameplayEffectExecute(struct FGameplayEffectModCallbackData& Data)
{
	// Return false if Target has immune TAG.
	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		if (Data.Target.HasMatchingGameplayTag(TAG_Gameplay_DamageImmunity))
		{
			Data.EvaluatedData.Magnitude = 0.f;
			return false;
		}
	}
	
	HealthBeforeAttributeChange = GetHealth();
	MaxHealthBeforeAttributeChange = GetMaxHealth();
	
	return true;
}

void UKernelHealthAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	
	const FGameplayEffectContextHandle& EffectContext = Data.EffectSpec.GetEffectContext();
	AActor* Instigator = EffectContext.GetOriginalInstigator();
	AActor* Causer = EffectContext.GetEffectCauser();
	
	float MinimumHealth = 0.0f;
	
	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		if (Data.EvaluatedData.Magnitude > 0.0f)
		{
			UAbilitySystemComponent* SourceASC = EffectContext.GetInstigatorAbilitySystemComponent();
			
			FKernelVerbMessage Message;
			Message.Verb = TAG_Kernel_Damage_Message;
			Message.Instigator = SourceASC ? SourceASC->GetOwnerActor() : Instigator;
			Message.Target = GetOwningActor();
			Message.Magnitude = Data.EvaluatedData.Magnitude;

			UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());
			MessageSystem.BroadcastMessage(Message.Verb, Message);
			
			/*
			UE_LOG(LogTemp, Warning, TEXT("[DamageMsg] Broadcast (Causer : %s) Target=%s Dmg=%f"),
			*GetNameSafe(Message.Instigator), *GetNameSafe(Message.Target), Message.Magnitude);
			*/
	
			// 최종 데미지값(랜덤 적용 완료)으로 DamagePop 큐를 쏜다.
			if (UAbilitySystemComponent* TargetASC = GetOwningAbilitySystemComponent())
			{
				FGameplayTagContainer AssetTags;
				Data.EffectSpec.GetAllAssetTags(AssetTags);
				
				FGameplayCueParameters PopParams;
				PopParams.RawMagnitude = Data.EvaluatedData.Magnitude; // ← 진짜 최종 데미지
				PopParams.Location = GetOwningActor()->GetActorLocation();
				PopParams.EffectContext = Data.EffectSpec.GetEffectContext();
				PopParams.EffectCauser = Causer; // 색상 판별용 (아바타 폰)

				PopParams.AggregatedSourceTags = 
					AssetTags.Filter(FGameplayTagContainer(FGameplayTag::RequestGameplayTag("Gameplay.DamageType")));
				
				TargetASC->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.DamagePop"), PopParams);
			}
		}
		
		// Convert into -Health and then clamp
		SetHealth(FMath::Clamp(GetHealth() - GetDamage(), MinimumHealth, GetMaxHealth()));
		UE_LOG(LogAbilitySystem, Warning, TEXT("[HealthSet] %s Health : %f"), *GetOuter()->GetName(), GetHealth());
		SetDamage(0.0f);
	}
	else if (Data.EvaluatedData.Attribute == GetHealingAttribute())
	{
		// Convert into +Health and then clamp
		SetHealth(FMath::Clamp(GetHealth() + GetHealing(), MinimumHealth, GetMaxHealth()));
		SetHealing(0.0f);
	}
	else if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		// Clamp and fall into out of health handling below
		SetHealth(FMath::Clamp(GetHealth(), MinimumHealth, GetMaxHealth()));
	}
	else if (Data.EvaluatedData.Attribute == GetMaxHealthAttribute())
	{
		// TODO clamp current health?

		// Notify on any requested max health changes
		OnMaxHealthChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, MaxHealthBeforeAttributeChange, GetMaxHealth());
	}
	
	if (GetHealth() != HealthBeforeAttributeChange)
	{
		OnHealthChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, HealthBeforeAttributeChange, GetHealth());
	}
	
	if ((GetHealth() <= 0.0f) && !bOutOfHealth)
	{
		OnOutOfHealth.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, HealthBeforeAttributeChange, GetHealth());
		UE_LOG(LogTemp,Warning,TEXT("HealthSet :: %s Dead!"), *GetOwningActor()->GetName());
	}

	bOutOfHealth = (GetHealth() <= 0.0f);
}

void UKernelHealthAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
}

void UKernelHealthAttributeSet::OnRep_Health(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UKernelHealthAttributeSet, Health, OldValue);
	
	const float CurrentHealth = GetHealth();
	const float EstimatedMagnitude = CurrentHealth - OldValue.GetCurrentValue();
	
	OnHealthChanged.Broadcast(nullptr, nullptr, nullptr, EstimatedMagnitude, OldValue.GetCurrentValue(), CurrentHealth);

	if (!bOutOfHealth && CurrentHealth <= 0.0f)
	{
		OnOutOfHealth.Broadcast(nullptr, nullptr, nullptr, EstimatedMagnitude, OldValue.GetCurrentValue(), CurrentHealth);
	}

	bOutOfHealth = (CurrentHealth <= 0.0f);
}

void UKernelHealthAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UKernelHealthAttributeSet, MaxHealth, OldValue);
	
	OnMaxHealthChanged.Broadcast(nullptr, nullptr, nullptr, GetMaxHealth() - OldValue.GetCurrentValue(), OldValue.GetCurrentValue(), GetMaxHealth());
}
