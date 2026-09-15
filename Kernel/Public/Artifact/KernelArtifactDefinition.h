// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "Item/KernelItemTypes.h"
#include "KernelArtifactDefinition.generated.h"

class UKernelAbilitySet;

UENUM()
enum class EKernelArtifactType : uint8
{
	None,
	Movement,
	Melee,
	Ranged,
	Magic,
	Attack,
	Defend,
	Special,
};

UCLASS()
class KERNEL_API UKernelArtifactDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly) FText DisplayName;
	UPROPERTY(EditDefaultsOnly) FText Description;
	UPROPERTY(EditDefaultsOnly) TArray<EKernelArtifactType> Types;
	UPROPERTY(EditDefaultsOnly) TSoftObjectPtr<UTexture2D> Icon;
	UPROPERTY(EditDefaultsOnly) FGameplayTag ArtifactTag;   // 시너지 판정용
	UPROPERTY(EditDefaultsOnly) EItemRarity Rarity;
	UPROPERTY(EditDefaultsOnly) bool bUnique = false;

	/** 이 아티팩트가 부여하는 것 — 이미 있는 AbilitySet을 그대로 쓴다 */
	UPROPERTY(EditDefaultsOnly) TObjectPtr<UKernelAbilitySet> GrantSet;
};
