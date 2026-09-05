// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "KernelAbilitySet.generated.h"

struct FGameplayAbilitySpecHandle;
class UAbilitySystemComponent;
class UGameplayAbility;

USTRUCT(BlueprintType)
struct FHeroAbilitySet
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	TSubclassOf<UGameplayAbility> Ability;
	
	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	int32 AbilityLevel = 1;
	
	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	FGameplayTag InputTag;
};
UCLASS()
class KERNEL_API UKernelAbilitySet : public UDataAsset
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
	TArray<FHeroAbilitySet> GrantedAbilities;
	
public:
	void GiveToAbilitySystem(
		UAbilitySystemComponent* ASC, 
		TArray<FGameplayAbilitySpecHandle>* OutGrantedHandles,
		UObject* SourceObject) const;
};