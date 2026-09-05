// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "KernelHealthComponent.generated.h"

struct FGameplayEffectSpec;
class UKernelAbilitySystemComponent;
class UAbilitySystemComponent;
class UKernelHealthAttributeSet;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(
	FKernelHealth_AttributeChanged, 
	UKernelHealthComponent*, HealthComponent, 
	float, OldValue, 
	float, NewValue, 
	AActor*, Instigator
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeathStarted, AActor*, DeadEnemy);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KERNEL_API UKernelHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UKernelHealthComponent();
	
	void InitializeWithAbilitySystem(UKernelAbilitySystemComponent* InASC);
	
	UPROPERTY(BlueprintAssignable)
	FKernelHealth_AttributeChanged OnHealthChanged;
	
	FOnDeathStarted OnDeathStarted;
	
	UFUNCTION(BlueprintCallable, Category = "Kernel|Health")
	float GetHealthPercent() const;
	
protected:
	void HandleHealthChanged(
		AActor* Instigator, 
		AActor* Causer, 
		const FGameplayEffectSpec* EffectSpec, 
		float Magnitude, 
		float OldValue, 
		float NewValue
	);
	
	void HandleOutOfHealth(
		AActor* DamageInstigator, 
		AActor* DamageCauser,
		const FGameplayEffectSpec* DamageEffectSpec,
		float DamageMagnitude,
		float OldValue,
		float NewValue);
	
	UPROPERTY(EditAnywhere)
	float HealthAmount = 100.f;
private:
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> CachedASC;

	UPROPERTY()
	const UKernelHealthAttributeSet* HealthSet;
};
