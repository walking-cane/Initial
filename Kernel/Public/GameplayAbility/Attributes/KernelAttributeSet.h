// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "KernelAttributeSet.generated.h"

struct FGameplayEffectSpec;

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

DECLARE_MULTICAST_DELEGATE_SixParams(
	FKernelAttributeEvent,
	AActor* /*EffectInstigator*/,
	AActor* /*EffectCauser*/,
	const FGameplayEffectSpec* /*EffectSpec*/,
	float /*EffectMagnitude*/,
	float /*OldValue*/,
	float /*NewValue*/);

UCLASS()
class KERNEL_API UKernelAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UKernelAttributeSet();
};
